/**
 * @file ioapic.h
 * @brief I/O APIC-Treiber für PinguinOS – Feature S7.
 *
 * Der I/O APIC (Intel 82093AA) verteilt externe Hardware-Interrupts
 * (IRQs) an die Local-APICs der einzelnen CPUs.  Er ersetzt den
 * veralteten 8259-PIC (Programmable Interrupt Controller) und
 * ermöglicht symmetrische Interrupt-Verteilung in SMP-Systemen.
 *
 * Standardmäßige physische MMIO-Adresse: 0xFEC00000
 *
 * Registerzugriff:
 *   IOREGSEL (Offset 0x00)  – Registerauswahl
 *   IOWIN    (Offset 0x10)  – Datenfenster
 */
#ifndef _IOAPIC_H
#define _IOAPIC_H

#include "types.h"

/* ── Physische MMIO-Basisadresse ─────────────────────────────────── */
#define IOAPIC_BASE_PHYS  0xFEC00000u

/* ── I/O APIC-Register-Indizes ───────────────────────────────────── */
#define IOAPIC_REG_ID       0x00   /* I/O APIC-ID                   */
#define IOAPIC_REG_VER      0x01   /* I/O APIC-Version              */
#define IOAPIC_REG_ARB      0x02   /* Arbitrierungspriorität        */
/* Redirect-Tabelle: 2 × 32-Bit pro Eintrag, Einträge 0..23         */
#define IOAPIC_REG_REDIR(n) (0x10 + (n)*2)

/* ── Redirect-Tabellen-Eintrag (64-Bit) ──────────────────────────── */
/* Unteres 32-Bit-Word */
#define IOAPIC_RTE_VECTOR_MASK  0x000000FFu   /* Interrupt-Vektor    */
#define IOAPIC_RTE_DELMODE_MASK 0x00000700u   /* Zustellmodus        */
#define IOAPIC_RTE_DESTMOD      (1u <<  11)   /* 0=physisch, 1=logisch */
#define IOAPIC_RTE_POLARITY     (1u <<  13)   /* 0=hoch-aktiv, 1=tief */
#define IOAPIC_RTE_TRIGGER      (1u <<  15)   /* 0=Flanke, 1=Pegel   */
#define IOAPIC_RTE_MASKED       (1u <<  16)   /* 1=Interrupt maskiert */
/* Oberes 32-Bit-Word */
#define IOAPIC_RTE_DEST_SHIFT   24            /* Ziel-APIC-ID (8 Bit) */

/* Zustellmodi */
#define IOAPIC_DELMODE_FIXED    0x00   /* Feste Zustellung           */
#define IOAPIC_DELMODE_LOWEST   0x01   /* Niedrigste Priorität       */
#define IOAPIC_DELMODE_SMI      0x02   /* System-Management-Interrupt */
#define IOAPIC_DELMODE_NMI      0x04   /* Non-Maskable Interrupt     */
#define IOAPIC_DELMODE_INIT     0x05   /* INIT-Signal                */
#define IOAPIC_DELMODE_EXTINT   0x07   /* Externer 8259-Modus        */

/* ── Maximale Anzahl IRQ-Leitungen ──────────────────────────────── */
#define IOAPIC_MAX_IRQS  24

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief I/O APIC initialisieren.
 *
 * Mappt das MMIO-Register, liest ID und Version, maskiert alle
 * Interrupt-Eingänge und richtet die Standard-IRQ-Zuordnung ein.
 * Muss nach apic_init() und vor der Aktivierung externer Interrupts
 * aufgerufen werden.
 */
void ioapic_init(void);

/**
 * @brief Einen IRQ in der Redirect-Tabelle konfigurieren.
 *
 * @param irq       Hardware-IRQ-Nummer (0–23).
 * @param vector    Ziel-Interrupt-Vektor (0x20–0xFF).
 * @param dest_apic Ziel-APIC-ID (typisch 0 für BSP).
 * @param flags     Kombination aus IOAPIC_RTE_*-Flags.
 */
void ioapic_set_redir(uint8_t irq, uint8_t vector,
                      uint8_t dest_apic, uint32_t flags);

/**
 * @brief Einen IRQ maskieren (deaktivieren).
 * @param irq  Hardware-IRQ-Nummer (0–23).
 */
void ioapic_mask(uint8_t irq);

/**
 * @brief Einen IRQ demaskieren (aktivieren).
 * @param irq  Hardware-IRQ-Nummer (0–23).
 */
void ioapic_unmask(uint8_t irq);

/** @return I/O APIC-ID aus dem ID-Register. */
uint8_t ioapic_get_id(void);

/** @return Maximale Anzahl unterstützter IRQ-Eingänge. */
uint8_t ioapic_max_irqs(void);

/** I/O APIC-Status auf seriell ausgeben. */
void ioapic_dump(void);

#endif /* _IOAPIC_H */