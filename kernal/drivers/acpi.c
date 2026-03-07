/**
 * @file acpi.c
 * @brief ACPI-Parser und Power-Management für PinguinOS.
 */
#include "../include/acpi.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/vga.h"
#include "../include/keyboard.h"

static acpi_state_t acpi_state;

/* ── Checksumme prüfen ───────────────────────────────────────────── */
static bool acpi_checksum(const void *data, size_t len)
{
    uint8_t sum = 0;
    const uint8_t *p = (const uint8_t *)data;
    for (size_t i = 0; i < len; i++) sum += p[i];
    return sum == 0;
}

/* ── RSDP in BIOS-Bereichen suchen ──────────────────────────────── */
static rsdp_t *rsdp_find(void)
{
    /* Suchbereiche: EBDA (1 KB ab 0x40:0x0E) und 0xE0000–0xFFFFF */
    uint32_t ranges[][2] = {
        { 0x000E0000, 0x000FFFFF },
        { 0x00080000, 0x0009FFFF },
    };

    for (int r = 0; r < 2; r++) {
        for (uint32_t addr = ranges[r][0];
             addr < ranges[r][1];
             addr += 16)
        {
            rsdp_t *rsdp = (rsdp_t *)addr;
            if (memcmp(rsdp->signature, ACPI_SIG_RSDP, 8) != 0) continue;
            if (!acpi_checksum(rsdp, 20)) continue;
            return rsdp;
        }
    }
    return NULL;
}

/* ── RSDT nach einer Tabellen-Signatur durchsuchen ───────────────── */
static acpi_header_t *rsdt_find_table(const char *sig)
{
    if (!acpi_state.rsdt_phys) return NULL;

    acpi_header_t *rsdt = (acpi_header_t *)acpi_state.rsdt_phys;
    uint32_t count = (rsdt->length - sizeof(acpi_header_t)) / 4;
    uint32_t *entries = (uint32_t *)(acpi_state.rsdt_phys + sizeof(acpi_header_t));

    for (uint32_t i = 0; i < count; i++) {
        acpi_header_t *tbl = (acpi_header_t *)entries[i];
        if (!tbl) continue;
        if (memcmp(tbl->signature, sig, 4) == 0)
            return tbl;
    }
    return NULL;
}

/* ── FADT parsen: PM-Control-Register-Adressen extrahieren ──────── */
static void parse_fadt(void)
{
    acpi_header_t *hdr = rsdt_find_table(ACPI_SIG_FADT);
    if (!hdr) { KWARN("ACPI: keine FADT gefunden\n"); return; }

    acpi_state.fadt_phys = (uint32_t)hdr;
    fadt_t *fadt = (fadt_t *)hdr;

    acpi_state.pm1a_control = (uint16_t)fadt->pm1a_control_blk;
    acpi_state.pm1b_control = (uint16_t)fadt->pm1b_control_blk;

    /* S5-Schlaftyp aus dem DSDT lesen (vereinfacht: Standardwert) */
    acpi_state.slp_typa = 5 << 10;   /* SLP_TYP für S5 */
    acpi_state.slp_typb = 5 << 10;

    KINFO("ACPI: FADT @ 0x%08x  PM1a=0x%04x  PM1b=0x%04x\n",
          (uint32_t)hdr, acpi_state.pm1a_control, acpi_state.pm1b_control);
}

/* ── HPET-Tabelle auslesen ───────────────────────────────────────── */
static void parse_hpet(void)
{
    /* Vereinfachter HPET-Tabellen-Header */
    typedef struct {
        acpi_header_t header;
        uint32_t      event_timer_blk_id;
        uint8_t       address_space;    /* 0=Sys-Mem */
        uint8_t       register_bit_width;
        uint8_t       register_bit_offset;
        uint8_t       reserved;
        uint64_t      base_address;
        uint8_t       hpet_number;
        uint16_t      minimum_tick;
        uint8_t       page_protection;
    } PACKED hpet_acpi_t;

    acpi_header_t *hdr = rsdt_find_table(ACPI_SIG_HPET);
    if (!hdr) return;

    hpet_acpi_t *hpet = (hpet_acpi_t *)hdr;
    acpi_state.hpet_phys = (uint32_t)hpet->base_address;
    KINFO("ACPI: HPET @ 0x%08x\n", acpi_state.hpet_phys);
}

/* ── Öffentlich: acpi_init ───────────────────────────────────────── */
void acpi_init(void)
{
    memset(&acpi_state, 0, sizeof(acpi_state));

    rsdp_t *rsdp = rsdp_find();
    if (!rsdp) {
        KWARN("ACPI: kein RSDP gefunden – ACPI deaktiviert\n");
        return;
    }

    acpi_state.rsdt_phys = rsdp->rsdt_address;
    KINFO("ACPI: RSDP @ %p  RSDT @ 0x%08x  Rev %u\n",
          (void *)rsdp, acpi_state.rsdt_phys, rsdp->revision);

    parse_fadt();
    parse_hpet();

    acpi_state.initialised = true;
    KINFO("ACPI: initialisiert\n");
}

const acpi_state_t *acpi_get_state(void) { return &acpi_state; }

void acpi_dump(void)
{
    KINFO("ACPI-Zustand:\n");
    KINFO("  RSDT=0x%08x  FADT=0x%08x  HPET=0x%08x\n",
          acpi_state.rsdt_phys, acpi_state.fadt_phys, acpi_state.hpet_phys);
    KINFO("  PM1a=0x%04x  PM1b=0x%04x\n",
          acpi_state.pm1a_control, acpi_state.pm1b_control);
}

/* ── Shutdown ────────────────────────────────────────────────────── */
void acpi_shutdown(void)
{
    vga_set_color(VGA_WHITE, VGA_BLUE);
    vga_puts("\n  System wird heruntergefahren…\n");
    serial_puts("[ACPI] Shutdown\n");

    if (acpi_state.pm1a_control) {
        /* Bit 13 = SLP_EN, Bits 10:12 = SLP_TYP (S5) */
        uint16_t val = (uint16_t)(acpi_state.slp_typa | (1 << 13));
        outw(acpi_state.pm1a_control, val);
        if (acpi_state.pm1b_control)
            outw(acpi_state.pm1b_control,
                 (uint16_t)(acpi_state.slp_typb | (1 << 13)));
    }

    /* Fallback für QEMU/Bochs: IO-Port 0x604 */
    outw(0x0604, 0x2000);

    cli();
    for (;;) hlt();
}

/* ── Reboot ──────────────────────────────────────────────────────── */
void acpi_reboot(void)
{
    serial_puts("[ACPI] Reboot\n");

    /* Methode 1: Keyboard-Controller (universell) */
    while (inb(PS2_STATUS_PORT) & 0x02);
    outb(PS2_CMD_PORT, 0xFE);   /* Pulse Reset-Leitung */
    io_wait();

    /* Methode 2: Triple Fault via ungültigen IDT-Load */
    typedef struct { uint16_t limit; uint32_t base; } PACKED idtr_t;
    idtr_t bad = { 0, 0 };
    __asm__ volatile ("lidt (%0); int $3" :: "r"(&bad));

    for (;;) hlt();
}


