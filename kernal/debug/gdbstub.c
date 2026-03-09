/**
 * @file gdbstub.c
 * @brief GDB Remote Serial Protocol (RSP) Stub – Feature #93.
 *
 * Kommuniziert über COM1 (9600 Baud, kein Parity, 1 Stop-Bit).
 * Das GDB RSP-Protokoll verwendet Pakete der Form: $Daten#KS
 * wobei KS die 1-Byte-Hexprüfsumme aller Daten-Bytes ist.
 *
 * Einschränkungen von PinguinOS 2.3:
 *   - Nur 32-Bit x86-Register
 *   - Kein Multi-Threading-Unterstützung (immer Thread 1)
 *   - Speicherzugriffe ohne Seitenvalidierung (kann Page-Fault auslösen)
 */

#include "../include/gdbstub.h"
#include "../include/idt.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/cpu.h"

/* ── GDB-RSP Protokollzeichen ────────────────────────────────── */
#define GDB_START   '$'
#define GDB_END     '#'
#define GDB_ACK     '+'
#define GDB_NAK     '-'
#define GDB_ESCAPE  '}'

/* ── Puffergröße ─────────────────────────────────────────────── */
#define GDB_BUF_SIZE  512

/* ── Zustand ─────────────────────────────────────────────────── */
static bool        gdb_aktiv     = false;   /* true wenn GDB wartet      */
static bool        gdb_verbunden = false;   /* true nach erstem Paket    */
static cpu_regs_t *halt_regs     = NULL;    /* Register beim Halt        */
static int         halt_signal   = 5;       /* SIGTRAP                   */

/* ── Hex-Hilfsfunktionen ─────────────────────────────────────── */
static INLINE char nibble_zu_hex(uint8_t n)
{ return (char)(n < 10 ? '0'+n : 'a'+n-10); }

static INLINE int hex_zu_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

/* uint32 als 8 Hex-Zeichen (Little-Endian) in buf schreiben */
static int u32_le_hex(char *buf, uint32_t val)
{
    for (int i = 0; i < 4; i++) {
        uint8_t b = (uint8_t)(val >> (i * 8));
        buf[i*2]   = nibble_zu_hex(b >> 4);
        buf[i*2+1] = nibble_zu_hex(b & 0xF);
    }
    return 8;
}

/* ── Seriell lesen/schreiben ─────────────────────────────────── */
static char gdb_getc(void) { return serial_getc(); }
static void gdb_putc(char c) { serial_putc(c); }

static void gdb_puts_raw(const char *s)
{ while (*s) gdb_putc(*s++); }

/* ── RSP-Paket senden ────────────────────────────────────────── */
static void gdb_send_packet(const char *daten)
{
    /* Prüfsumme berechnen */
    uint8_t ks = 0;
    for (const char *p = daten; *p; p++) ks += (uint8_t)*p;

    /* Solange senden bis ACK empfangen */
    for (;;) {
        gdb_putc(GDB_START);
        gdb_puts_raw(daten);
        gdb_putc(GDB_END);
        gdb_putc(nibble_zu_hex(ks >> 4));
        gdb_putc(nibble_zu_hex(ks & 0xF));

        char antwort = gdb_getc();
        if (antwort == GDB_ACK) break;
        /* Bei NAK erneut senden */
    }
}

/* ── RSP-Paket empfangen ─────────────────────────────────────── */
static int gdb_recv_packet(char *buf, uint32_t maxlen)
{
    /* Auf Paketbeginn warten */
    char c;
    do { c = gdb_getc(); } while (c != GDB_START);

    /* Daten lesen bis '#' */
    uint32_t len = 0;
    uint8_t  ks  = 0;
    while ((c = gdb_getc()) != GDB_END && len + 1 < maxlen) {
        buf[len++] = c;
        ks += (uint8_t)c;
    }
    buf[len] = '\0';

    /* Prüfsumme lesen und vergleichen */
    char h1 = gdb_getc(), h2 = gdb_getc();
    int erw = (hex_zu_nibble(h1) << 4) | hex_zu_nibble(h2);
    if ((uint8_t)erw != ks) {
        gdb_putc(GDB_NAK);
        return -1;
    }
    gdb_putc(GDB_ACK);
    gdb_verbunden = true;
    return (int)len;
}

/* ── Haupt-GDB-Schleife ──────────────────────────────────────── */
static void gdb_schleife(void)
{
    char paket[GDB_BUF_SIZE];
    char antwort[GDB_BUF_SIZE];
    gdb_aktiv = true;

    /* Halt-Signal senden */
    snprintf(antwort, sizeof(antwort), "S%02x", halt_signal);
    gdb_send_packet(antwort);

    while (gdb_aktiv) {
        if (gdb_recv_packet(paket, sizeof(paket)) < 0) continue;

        antwort[0] = '\0';

        switch (paket[0]) {
        case '?':
            /* Halt-Grund abfragen */
            snprintf(antwort, sizeof(antwort), "S%02x", halt_signal);
            break;

        case 'g':
            /* Alle Register lesen (8 GPRs + EIP + EFLAGS = 10 × 32 Bit) */
            if (halt_regs) {
                int pos = 0;
                /* GDB-Reihenfolge: eax ecx edx ebx esp ebp esi edi eip eflags */
                pos += u32_le_hex(antwort+pos, halt_regs->eax);
                pos += u32_le_hex(antwort+pos, halt_regs->ecx);
                pos += u32_le_hex(antwort+pos, halt_regs->edx);
                pos += u32_le_hex(antwort+pos, halt_regs->ebx);
                pos += u32_le_hex(antwort+pos, halt_regs->esp);
                pos += u32_le_hex(antwort+pos, halt_regs->ebp);
                pos += u32_le_hex(antwort+pos, halt_regs->esi);
                pos += u32_le_hex(antwort+pos, halt_regs->edi);
                pos += u32_le_hex(antwort+pos, halt_regs->eip);
                pos += u32_le_hex(antwort+pos, halt_regs->eflags);
                antwort[pos] = '\0';
            } else {
                strncpy(antwort, "00000000", sizeof(antwort));
            }
            break;

        case 'm': {
            /* Speicher lesen: m Adresse,Länge */
            char *komma = strchr(paket + 1, ',');
            if (!komma) { strncpy(antwort, "E01", sizeof(antwort)); break; }
            uint32_t addr = strtoul(paket + 1, NULL, 16);
            uint32_t len  = strtoul(komma + 1, NULL, 16);
            if (len > 128) len = 128;
            int pos = 0;
            for (uint32_t i = 0; i < len; i++) {
                uint8_t b = *(uint8_t *)(addr + i);
                antwort[pos++] = nibble_zu_hex(b >> 4);
                antwort[pos++] = nibble_zu_hex(b & 0xF);
            }
            antwort[pos] = '\0';
            break;
        }

        case 'c':
            /* Ausführung fortsetzen */
            gdb_aktiv = false;
            strncpy(antwort, "OK", sizeof(antwort));
            break;

        case 's':
            /* Einzelschritt: TF-Bit setzen und fortsetzen */
            if (halt_regs) halt_regs->eflags |= (1u << 8);
            gdb_aktiv = false;
            strncpy(antwort, "OK", sizeof(antwort));
            break;

        case 'q':
            /* Abfragen */
            if (strncmp(paket+1, "Supported", 9) == 0)
                strncpy(antwort, "PacketSize=200", sizeof(antwort));
            else if (strncmp(paket+1, "Attached", 8) == 0)
                strncpy(antwort, "1", sizeof(antwort));
            break;

        default:
            /* Unbekanntes Paket: leere Antwort */
            antwort[0] = '\0';
            break;
        }

        gdb_send_packet(antwort);
    }
}

/* ── Ausnahme-Handler ────────────────────────────────────────── */
static void gdb_int3_handler(cpu_regs_t *regs)
{
    halt_regs  = regs;
    halt_signal = 5;   /* SIGTRAP */
    gdb_schleife();
}

static void gdb_debug_handler(cpu_regs_t *regs)
{
    /* Einzelschritt-Ausnahme: TF-Bit löschen */
    regs->eflags &= ~(1u << 8);
    halt_regs   = regs;
    halt_signal = 5;
    gdb_schleife();
}

/* ── Öffentlich: gdbstub_init ────────────────────────────────── */
void gdbstub_init(void)
{
    /* INT3 (#BP) und #DB-Handler registrieren */
    exception_register(1, gdb_debug_handler);
    exception_register(3, gdb_int3_handler);
    KINFO("GDB-Stub: bereit  (COM1, INT3/DEBUG-Handler)\n");
}

void gdbstub_breakpoint(void)
{
    __asm__ volatile ("int $3");
}

bool gdbstub_connected(void) { return gdb_verbunden; }