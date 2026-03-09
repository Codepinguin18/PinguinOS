/**
 * @file smp.h
 * @brief Symmetric Multiprocessing (SMP) für PinguinOS – Feature #20.
 *
 * Unterstützt das Starten von Application Processors (APs) über den
 * APIC-IPI-Mechanismus (INIT–SIPI–SIPI).  Der Bootstrap Processor (BSP)
 * initialisiert alle APs, die anschließend im Kernel-Modus warten.
 *
 * Maximal unterstützte CPUs: SMP_MAX_CPUS
 */
#ifndef _SMP_H
#define _SMP_H

#include "types.h"

/* ── Konfiguration ───────────────────────────────────────────────── */
#define SMP_MAX_CPUS   8     /* Maximale Anzahl logischer Prozessoren */

/* ── CPU-Zustände ────────────────────────────────────────────────── */
typedef enum {
    CPU_STATE_OFFLINE = 0,   /* Nicht gestartet / nicht vorhanden     */
    CPU_STATE_INIT    = 1,   /* INIT-IPI gesendet                     */
    CPU_STATE_SIPI    = 2,   /* SIPI gesendet, AP bootet              */
    CPU_STATE_ONLINE  = 3,   /* AP ist bereit und läuft               */
} cpu_state_t;

/* ── Pro-CPU-Datenblock ──────────────────────────────────────────── */
typedef struct cpu_info_smp {
    uint8_t      apic_id;    /* Local-APIC-ID dieser CPU              */
    uint8_t      cpu_index;  /* Logischer CPU-Index (0 = BSP)         */
    cpu_state_t  state;      /* Aktueller Zustand                     */
    uint32_t     kstack_top; /* Kernel-Stack-Spitze für diesen AP     */
    uint64_t     total_ticks;/* Gesamte Scheduler-Ticks dieser CPU    */
} cpu_info_smp_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief SMP-Subsystem initialisieren.
 *
 * Liest die MADT (ACPI Multiple APIC Description Table), enumeriert
 * alle Application Processors und startet sie über INIT–SIPI–SIPI.
 * Muss nach apic_init() und acpi_init() aufgerufen werden.
 */
void smp_init(void);

/** @return Anzahl der aktuell online befindlichen CPUs (≥ 1). */
uint32_t smp_cpu_count(void);

/** @return APIC-ID der aktuell ausführenden CPU. */
uint8_t smp_current_cpu(void);

/** @return Zeiger auf den Datenblock der angegebenen CPU. */
const cpu_info_smp_t *smp_get_cpu(uint32_t index);

/**
 * @brief Inter-Processor Interrupt (IPI) an eine CPU senden.
 *
 * @param apic_id   Ziel-APIC-ID.
 * @param vector    Interrupt-Vektor (0x20–0xFF).
 */
void smp_send_ipi(uint8_t apic_id, uint8_t vector);

/**
 * @brief TLB-Shootdown an alle anderen CPUs senden.
 *
 * Schickt einen IPI an alle Online-CPUs außer der eigenen, damit
 * diese ihren TLB für die geänderte Seite ungültig machen.
 */
void smp_tlb_shootdown(uint32_t vaddr);

/** SMP-Status auf dem seriellen Port ausgeben. */
void smp_dump(void);

/* ── Trampolin-Adresse ───────────────────────────────────────────── */
/** Physische Adresse des AP-Trampolin-Codes (muss < 1 MB sein). */
#define SMP_TRAMPOLINE_ADDR  0x8000

#endif /* _SMP_H */