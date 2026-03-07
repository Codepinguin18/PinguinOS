/**
 * @file mouse.c
 * @brief PS/2 Mouse driver for PinguinOS.
 *
 * Feature #32 – PS/2 Mouse
 *
 * Protocol:
 *   The PS/2 controller sends 3 bytes per packet via IRQ12:
 *     Byte 0: status  (buttons + overflow/sign bits)
 *     Byte 1: delta X (signed 9-bit with sign in byte 0 bit 4)
 *     Byte 2: delta Y (signed 9-bit with sign in byte 0 bit 5, Y-down)
 *
 * We keep a small circular event queue (MOUSE_QUEUE_SIZE entries) so
 * higher layers can call mouse_poll() at their own pace.
 */

#include "../include/mouse.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── I/O ports ───────────────────────────────────────────────────── */
#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

#define PS2_STATUS_INPUT_FULL   (1 << 1)
#define PS2_STATUS_OUTPUT_FULL  (1 << 0)
#define PS2_STATUS_MOUSE_DATA   (1 << 5)

/* ── Event queue ─────────────────────────────────────────────────── */
#define MOUSE_QUEUE_SIZE  64

static mouse_event_t evt_queue[MOUSE_QUEUE_SIZE];
static volatile uint32_t q_head = 0;   /* Producer index */
static volatile uint32_t q_tail = 0;   /* Consumer index */

/* ── Packet assembly state ───────────────────────────────────────── */
static uint8_t  pkt[3];
static uint8_t  pkt_idx = 0;

/* ── Absolute position + button state ───────────────────────────── */
static int32_t mouse_x      = 0;
static int32_t mouse_y      = 0;
static uint8_t mouse_buttons = 0;
static int32_t screen_w     = 80;   /* Default VGA text width  */
static int32_t screen_h     = 25;   /* Default VGA text height */

/* ── PS/2 controller helpers ─────────────────────────────────────── */
static void ps2_wait_write(void)
{
    uint32_t timeout = 100000;
    while (timeout-- && (inb(PS2_STATUS) & PS2_STATUS_INPUT_FULL));
}

static void ps2_wait_read(void)
{
    uint32_t timeout = 100000;
    while (timeout-- && !(inb(PS2_STATUS) & PS2_STATUS_OUTPUT_FULL));
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

static uint8_t ps2_read(void)
{
    ps2_wait_read();
    return inb(PS2_DATA);
}

static void mouse_write_cmd(uint8_t cmd)
{
    /* Route command to the second PS/2 port */
    ps2_cmd(0xD4);
    ps2_write(cmd);
    ps2_read();   /* ACK */
}

/* ── IRQ12 handler ───────────────────────────────────────────────── */
static void mouse_irq_handler(cpu_regs_t *regs)
{
    (void)regs;

    uint8_t status = inb(PS2_STATUS);
    if (!(status & PS2_STATUS_OUTPUT_FULL)) return;
    if (!(status & PS2_STATUS_MOUSE_DATA))  return;

    uint8_t byte = inb(PS2_DATA);
    pkt[pkt_idx++] = byte;

    if (pkt_idx < 3) return;
    pkt_idx = 0;

    /* Validate: bit 3 of status byte must always be set */
    if (!(pkt[0] & 0x08)) return;

    /* Overflow bits – discard packet if set */
    if (pkt[0] & 0xC0) return;

    /* Decode signed 9-bit deltas */
    int16_t dx = (int16_t)pkt[1] - ((pkt[0] & 0x10) ? 256 : 0);
    int16_t dy = (int16_t)pkt[2] - ((pkt[0] & 0x20) ? 256 : 0);
    dy = -dy;   /* PS/2 Y is inverted relative to screen coords */

    uint8_t buttons = pkt[0] & 0x07;

    /* Update absolute position */
    mouse_x += dx;
    mouse_y += dy;
    if (mouse_x < 0)           mouse_x = 0;
    if (mouse_x >= screen_w)   mouse_x = screen_w - 1;
    if (mouse_y < 0)           mouse_y = 0;
    if (mouse_y >= screen_h)   mouse_y = screen_h - 1;
    mouse_buttons = buttons;

    /* Enqueue event (drop if queue is full) */
    uint32_t next_head = (q_head + 1) % MOUSE_QUEUE_SIZE;
    if (next_head != q_tail) {
        evt_queue[q_head].dx      = dx;
        evt_queue[q_head].dy      = dy;
        evt_queue[q_head].buttons = buttons;
        q_head = next_head;
    }
}

/* ── Public: mouse_init ──────────────────────────────────────────── */
void mouse_init(void)
{
    /* Enable the auxiliary (mouse) port */
    ps2_cmd(0xA8);

    /* Read current compaq status byte, set bit 1 (enable IRQ12) */
    ps2_cmd(0x20);
    uint8_t status = ps2_read();
    status |= (1 << 1);    /* Enable IRQ12 */
    status &= ~(1 << 5);   /* Clear "disable mouse clock" */
    ps2_cmd(0x60);
    ps2_write(status);

    /* Reset mouse */
    mouse_write_cmd(0xFF);

    /* Set stream mode (default, but be explicit) */
    mouse_write_cmd(0xF6);   /* Set defaults */
    mouse_write_cmd(0xF4);   /* Enable data reporting */

    memset(pkt, 0, sizeof(pkt));
    pkt_idx = 0;
    q_head = q_tail = 0;

    irq_register(12, mouse_irq_handler);

    KINFO("Mouse: PS/2 mouse initialised (IRQ12)\n");
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