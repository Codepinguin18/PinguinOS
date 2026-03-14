/**
 * @file mouse.c
 * @brief PS/2 Mouse driver for PinguinOS.
 *
 * Fixes:
 *  - Korrekte Reset-Sequenz: liest alle 3 Antwort-Bytes (ACK + BAT + ID).
 *    Ohne diesen Fix korrumpieren die 2 übrigen Bytes alle folgenden
 *    ps2_read()-Aufrufe, und die Maus sendet danach nur Müll.
 *  - Bit-5-Check (PS2_STATUS_MOUSE_DATA) entfernt: In manchen QEMU-
 *    Versionen wird dieses Bit nicht gesetzt. Da wir auf IRQ12 sind,
 *    kommen sowieso nur Maus-Daten – der Check ist falsch-negativ.
 *  - Konsolen-Logging für Bewegungen und Klicks.
 *  - prev_buttons für Flanken-Erkennung (kein Spam bei gedrückter Taste).
 */

#include "../include/mouse.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/vmmouse.h"

/* ── I/O ports ───────────────────────────────────────────────────── */
#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

#define PS2_STATUS_INPUT_FULL   (1 << 1)   /* Controller bereit für Befehl */
#define PS2_STATUS_OUTPUT_FULL  (1 << 0)   /* Daten bereit zum Lesen       */

/* ── Event queue ─────────────────────────────────────────────────── */
#define MOUSE_QUEUE_SIZE  64

static mouse_event_t evt_queue[MOUSE_QUEUE_SIZE];
static volatile uint32_t q_head = 0;
static volatile uint32_t q_tail = 0;

/* ── Packet assembly state ───────────────────────────────────────── */
static uint8_t  pkt[3];
static uint8_t  pkt_idx = 0;

/* ── Absolute position + button state ───────────────────────────── */
static int32_t mouse_x       = 0;
static int32_t mouse_y       = 0;
static uint8_t mouse_buttons = 0;
static uint8_t prev_buttons  = 0;   /* Für Klick-Flanken-Erkennung */
static int32_t screen_w      = 1280;
static int32_t screen_h      = 720;

/* ── PS/2 controller helpers ─────────────────────────────────────── */
static void ps2_wait_write(void)
{
    uint32_t t = 100000;
    while (t-- && (inb(PS2_STATUS) & PS2_STATUS_INPUT_FULL));
}

static void ps2_cmd(uint8_t cmd)
{
    ps2_wait_write();
    outb(PS2_CMD, cmd);
}

static void ps2_write(uint8_t val)
{
    ps2_wait_write();
    outb(PS2_DATA, val);
}

/* Liest ein Byte mit Timeout; gibt 0 zurück falls kein Byte kommt */
static uint8_t ps2_read_timeout(void)
{
    uint32_t t = 100000;
    while (t-- && !(inb(PS2_STATUS) & PS2_STATUS_OUTPUT_FULL));
    if (!(inb(PS2_STATUS) & PS2_STATUS_OUTPUT_FULL)) return 0;
    return inb(PS2_DATA);
}

/* Sendet Befehl an Maus (Port 2) und liest nur das ACK-Byte */
static uint8_t mouse_send(uint8_t cmd)
{
    ps2_cmd(0xD4);          /* Nächstes Byte → Port 2 (Maus) */
    ps2_write(cmd);
    return ps2_read_timeout();   /* ACK lesen */
}

/* ── IRQ12 handler ───────────────────────────────────────────────── */
static void mouse_irq_handler(cpu_regs_t *regs)
{
    (void)regs;

    /* Nur lesen wenn Daten vorhanden – KEIN Bit-5-Check! */
    if (!(inb(PS2_STATUS) & PS2_STATUS_OUTPUT_FULL)) return;

    /* Falls VMMouse aktiv ist, versuchen wir absolute Koordinaten zu lesen.
       vmmouse_poll() liefert true, wenn ein Paket verarbeitet wurde. */
    if (vmmouse_poll()) {
        /* Paket wurde als absolutes Event verarbeitet, 
           wir leeren aber das PS/2 Byte damit IRQ gelöscht wird. */
        (void)inb(PS2_DATA);
        return;
    }

    uint8_t byte = inb(PS2_DATA);
    pkt[pkt_idx++] = byte;

    if (pkt_idx < 3) return;
    pkt_idx = 0;

    /* Bit 3 des Status-Bytes muss immer gesetzt sein */
    if (!(pkt[0] & 0x08)) return;

    /* Overflow-Bits: Paket verwerfen */
    if (pkt[0] & 0xC0) return;

    /* Signed 9-Bit Deltas dekodieren */
    int16_t dx = (int16_t)pkt[1] - ((pkt[0] & 0x10) ? 256 : 0);
    int16_t dy = (int16_t)pkt[2] - ((pkt[0] & 0x20) ? 256 : 0);
    dy = -dy;   /* PS/2 Y ist invertiert */

    uint8_t buttons = pkt[0] & 0x07;

    /* Absolute Position aktualisieren */
    mouse_x += dx;
    mouse_y += dy;
    if (mouse_x < 0)          mouse_x = 0;
    if (mouse_x >= screen_w)  mouse_x = screen_w - 1;
    if (mouse_y < 0)          mouse_y = 0;
    if (mouse_y >= screen_h)  mouse_y = screen_h - 1;
    mouse_buttons = buttons;

    /* Event in Queue einreihen */
    uint32_t next = (q_head + 1) % MOUSE_QUEUE_SIZE;
    if (next != q_tail) {
        evt_queue[q_head].dx       = dx;
        evt_queue[q_head].dy       = dy;
        evt_queue[q_head].abs_x    = mouse_x;
        evt_queue[q_head].abs_y    = mouse_y;
        evt_queue[q_head].buttons  = buttons;
        evt_queue[q_head].clicked  = buttons & ~prev_buttons;
        evt_queue[q_head].released = prev_buttons & ~buttons;
        q_head = next;
    }

    /* ── Konsolen-Logging ────────────────────────────────────────── */
    {
        char log[80];

        /* Bewegung nur loggen wenn tatsächlich bewegt */
        if (dx != 0 || dy != 0) {
            int adx = dx < 0 ? -dx : dx;
            int ady = dy < 0 ? -dy : dy;
            const char *dir = (adx >= ady)
                ? (dx > 0 ? "RECHTS" : "LINKS")
                : (dy > 0 ? "RUNTER" : "HOCH");
            snprintf(log, sizeof(log),
                     "[Maus] %s  dx=%d dy=%d px\n", dir, (int)dx, (int)dy);
            serial_puts(log);
        }

        /* Klicks nur bei neuer Flanke (Taste gedrückt, war vorher offen) */
        uint8_t pressed = buttons & ~prev_buttons;
        if (pressed & MOUSE_BTN_LEFT)   serial_puts("[Maus] Links-Klick\n");
        if (pressed & MOUSE_BTN_RIGHT)  serial_puts("[Maus] Rechts-Klick\n");
        if (pressed & MOUSE_BTN_MIDDLE) serial_puts("[Maus] Mitte-Klick\n");
        prev_buttons = buttons;
    }
}

/* ── Public: mouse_init ──────────────────────────────────────────── */
void mouse_init(void)
{
    pkt_idx = 0;
    q_head = q_tail = 0;
    prev_buttons = 0;

    /* ── Schritt 1: Maus-Port aktivieren ───────────────────────── */
    ps2_cmd(0xA8);

    /* ── Schritt 2: IRQ12 im Controller-Config-Byte aktivieren ─── */
    ps2_cmd(0x20);
    uint8_t cfg = ps2_read_timeout();
    cfg |=  (1 << 1);   /* IRQ12 einschalten   */
    cfg &= ~(1 << 5);   /* Mouse-Clock freigeben */
    ps2_cmd(0x60);
    ps2_write(cfg);

    /* ── Schritt 3: Maus zurücksetzen ──────────────────────────── */
    /* Reset sendet: ACK(0xFA) + BAT-Complete(0xAA) + Device-ID(0x00)
     * Alle 3 Bytes müssen gelesen werden, sonst korrumpieren 0xAA + 0x00
     * die nachfolgenden ps2_read()-Aufrufe für Set-Defaults und Enable! */
    mouse_send(0xFF);               /* Byte 1: ACK – schon von mouse_send gelesen */
    ps2_read_timeout();             /* Byte 2: BAT Complete (0xAA) verwerfen     */
    ps2_read_timeout();             /* Byte 3: Device ID   (0x00) verwerfen      */

    /* ── Schritt 4: Standard-Einstellungen + Reporting aktivieren ─ */
    mouse_send(0xF6);   /* Set Defaults */
    mouse_send(0xF4);   /* Enable Data Reporting */

    /* ── Schritt 5: IRQ12 registrieren ─────────────────────────── */
    irq_register(12, mouse_irq_handler);

    KINFO("Mouse: PS/2 Maus initialisiert (IRQ12, Reset-Fix aktiv)\n");
}

/* ── Public: mouse_poll ──────────────────────────────────────────── */
bool mouse_poll(mouse_event_t *evt)
{
    if (q_head == q_tail) return false;
    *evt   = evt_queue[q_tail];
    q_tail = (q_tail + 1) % MOUSE_QUEUE_SIZE;
    return true;
}

/* ── Public: accessors ───────────────────────────────────────────── */
int32_t mouse_get_x(void)       { return mouse_x; }
int32_t mouse_get_y(void)       { return mouse_y; }
uint8_t mouse_get_buttons(void) { return mouse_buttons; }

void mouse_set_bounds(int32_t width, int32_t height)
{
    screen_w = (width  > 0) ? width  : 1;
    screen_h = (height > 0) ? height : 1;
}

void mouse_set_abs_position(int32_t x, int32_t y, uint8_t buttons)
{
    /* VMMouse liefert meist 0..0xFFFF, wir skalieren auf screen_w/h */
    mouse_x = (x * screen_w) / 65535;
    mouse_y = (y * screen_h) / 65535;

    /* Klicks loggen (Flankenerkennung) */
    uint8_t pressed  = buttons & ~prev_buttons;
    uint8_t released = prev_buttons & ~buttons;
    if (pressed & MOUSE_BTN_LEFT)   serial_puts("[Maus] Abs-Links-Klick\n");
    if (pressed & MOUSE_BTN_RIGHT)  serial_puts("[Maus] Abs-Rechts-Klick\n");
    if (pressed & MOUSE_BTN_MIDDLE) serial_puts("[Maus] Abs-Mitte-Klick\n");

    /* Event in Queue (dx=0, dy=0 da wir absolute Werte setzen) */
    uint32_t next = (q_head + 1) % MOUSE_QUEUE_SIZE;
    if (next != q_tail) {
        evt_queue[q_head].dx       = 0;
        evt_queue[q_head].dy       = 0;
        evt_queue[q_head].abs_x    = mouse_x;
        evt_queue[q_head].abs_y    = mouse_y;
        evt_queue[q_head].buttons  = buttons;
        evt_queue[q_head].clicked  = pressed;
        evt_queue[q_head].released = released;
        q_head = next;
    }

    mouse_buttons = buttons;
    prev_buttons  = buttons;
}
