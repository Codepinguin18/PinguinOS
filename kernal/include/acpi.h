/**
 * @file acpi.h
 * @brief ACPI-Parser (Feature #35) und Power-Management/Shutdown (Feature #36).
 *
 * Implementiert:
 *  - RSDP-Suche im BIOS-Bereich
 *  - RSDT/XSDT-Parsing
 *  - FADT-Auswertung (PM1a/b Control Block für Shutdown)
 *  - Reboot über ACPI oder Keyboard-Controller
 */
#ifndef _ACPI_H
#define _ACPI_H

#include "types.h"

/* ── ACPI-Signaturen ─────────────────────────────────────────────── */
#define ACPI_SIG_RSDP  "RSD PTR "   /* 8 Bytes, kein Null-Terminator */
#define ACPI_SIG_RSDT  "RSDT"
#define ACPI_SIG_FADT  "FACP"
#define ACPI_SIG_HPET  "HPET"
#define ACPI_SIG_MADT  "APIC"

/* ── Gemeinsamer ACPI-Tabellen-Header ───────────────────────────── */
typedef struct acpi_header {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} PACKED acpi_header_t;

/* ── RSDP (Root System Description Pointer) ─────────────────────── */
typedef struct rsdp {
    char     signature[8];
    uint8_t  checksum;
    char     oem_id[6];
    uint8_t  revision;
    uint32_t rsdt_address;     /* Physische Adresse der RSDT           */
    /* ACPI 2.0+ Felder: */
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t  extended_checksum;
    uint8_t  reserved[3];
} PACKED rsdp_t;

/* ── FADT (Fixed ACPI Description Table) – relevante Felder ─────── */
typedef struct fadt {
    acpi_header_t header;
    uint32_t      firmware_ctrl;
    uint32_t      dsdt;
    uint8_t       _reserved;
    uint8_t       preferred_pm_profile;
    uint16_t      sci_interrupt;
    uint32_t      smi_cmd;
    uint8_t       acpi_enable;
    uint8_t       acpi_disable;
    uint8_t       s4bios_req;
    uint8_t       pstate_cnt;
    uint32_t      pm1a_event_blk;
    uint32_t      pm1b_event_blk;
    uint32_t      pm1a_control_blk;
    uint32_t      pm1b_control_blk;
    uint32_t      pm2_control_blk;
    uint32_t      pm_timer_blk;
    uint32_t      gpe0_blk;
    uint32_t      gpe1_blk;
    uint8_t       pm1_event_len;
    uint8_t       pm1_control_len;
    uint8_t       pm2_control_len;
    uint8_t       pm_timer_len;
    uint8_t       gpe0_len;
    uint8_t       gpe1_len;
    uint8_t       gpe1_base;
    uint8_t       cstate_control;
    uint16_t      worst_c2_latency;
    uint16_t      worst_c3_latency;
    uint16_t      flush_size;
    uint16_t      flush_stride;
    uint8_t       duty_offset;
    uint8_t       duty_width;
    uint8_t       day_alarm;
    uint8_t       month_alarm;
    uint8_t       century;
    uint16_t      boot_architecture_flags;
    uint8_t       reserved2;
    uint32_t      flags;
    /* ACPI 2.0+ fields */
    uint8_t       reset_reg_addr_space;
    uint8_t       reset_reg_bit_width;
    uint8_t       reset_reg_bit_offset;
    uint8_t       reset_reg_access_size;
    uint64_t      reset_reg_address;
    uint8_t       reset_value;
    uint8_t       reserved3[3];
} PACKED fadt_t;

#define ACPI_FADT_RESET_REG_SUP (1 << 10)

/* ── ACPI-Systemzustand ──────────────────────────────────────────── */
typedef struct acpi_state {
    bool      initialised;
    uint32_t  rsdt_phys;
    uint32_t  fadt_phys;
    uint32_t  hpet_phys;
    uint32_t  madt_phys;
    uint16_t  pm1a_control;   /* I/O-Port für PM1a-Control-Register   */
    uint16_t  pm1b_control;   /* 0 wenn nicht vorhanden               */
    uint16_t  slp_typa;       /* S5 Sleep-Type-A-Wert                 */
    uint16_t  slp_typb;
    /* Reset Register */
    uint64_t  reset_reg_addr;
    uint8_t   reset_value;
    bool      reset_supported;
} acpi_state_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** ACPI initialisieren: RSDP suchen, Tabellen parsen. */
void acpi_init(void);

/** ACPI-Systemzustand zurückgeben. */
const acpi_state_t *acpi_get_state(void);

/** Alle gefundenen Tabellen auf Serial ausgeben. */
void acpi_dump(void);

/** System sauber herunterfahren (ACPI S5). */
void NORETURN acpi_shutdown(void);

/** System neu starten (via Keyboard-Controller oder ACPI Reset). */
void NORETURN acpi_reboot(void);

#endif /* _ACPI_H */
