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
} PACKED fadt_t;

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
