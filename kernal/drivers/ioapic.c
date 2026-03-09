/**
 * @file ioapic.c
 * @brief I/O APIC-Treiber für PinguinOS – Feature S7.
 *
 * Vollständige Implementierung des I/O APIC (Intel 82093AA):
 *   – MMIO-Identitätsmapping (0xFEC00000)
 *   – Register-Zugriff über IOREGSEL/IOWIN
 *   – Initialisierung aller 24 IRQ-Leitungen (maskiert)
 *   – Standard-IRQ-Routing: IRQ0–IRQ15 → Vektor 0x20–0x2F, APIC-ID 0
 *   – Einzel-IRQ Maskierung/Demaskierung
 *
 * Voraussetzung: Der 8259-PIC muss zuvor vollständig maskiert worden
 * sein, damit keine Doppel-Interrupts auftreten.
 */

#include "../include/ioapic.h"
#include "../include/paging.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── MMIO-Basiszeiger ────────────────────────────────────────────── */
static volatile uint32_t *ioapic_base = NULL;
static uint8_t            ioapic_max  = 0;   /* Anzahl IRQ-Eingänge */

/* ── Registerzugriff über IOREGSEL / IOWIN ───────────────────────── */
static uint32_t ioapic_read(uint8_t reg)
{
    /* IOREGSEL: Offset 0x00 */
    ioapic_base[0] = (uint32_t)reg;
    /* Speicherbarriere: Schreibzugriff muss vor dem Lesen abgeschlossen sein */
    mb();
    /* IOWIN: Offset 0x10 → Index 4 im uint32_t-Array */
    return ioapic_base[4];
}

static void ioapic_write(uint8_t reg, uint32_t val)
{
    ioapic_base[0] = (uint32_t)reg;
    mb();
    ioapic_base[4] = val;
}

/* ── Redirect-Tabellen-Eintrag schreiben ─────────────────────────── */
static void ioapic_write_rte(uint8_t irq, uint32_t lo, uint32_t hi)
{
    ioapic_write((uint8_t)(IOAPIC_REG_REDIR(irq) + 1), hi);  /* Erst oberes Wort */
    ioapic_write((uint8_t) IOAPIC_REG_REDIR(irq),       lo); /* Dann unteres   */
}

static void ioapic_read_rte(uint8_t irq, uint32_t *lo, uint32_t *hi)
{
    *lo = ioapic_read((uint8_t) IOAPIC_REG_REDIR(irq));
    *hi = ioapic_read((uint8_t)(IOAPIC_REG_REDIR(irq) + 1));
}

/* ── Öffentlich: ioapic_set_redir ───────────────────────────────── */
void ioapic_set_redir(uint8_t irq, uint8_t vector,
                      uint8_t dest_apic, uint32_t flags)
{
    if (!ioapic_base || irq >= ioapic_max) return;

    uint32_t lo = (uint32_t)vector | flags;
    uint32_t hi = (uint32_t)dest_apic << IOAPIC_RTE_DEST_SHIFT;
    ioapic_write_rte(irq, lo, hi);
}

/* ── Öffentlich: ioapic_mask ─────────────────────────────────────── */
void ioapic_mask(uint8_t irq)
{
    if (!ioapic_base || irq >= ioapic_max) return;
    uint32_t lo, hi;
    ioapic_read_rte(irq, &lo, &hi);
    lo |= IOAPIC_RTE_MASKED;
    ioapic_write_rte(irq, lo, hi);
}

/* ── Öffentlich: ioapic_unmask ───────────────────────────────────── */
void ioapic_unmask(uint8_t irq)
{
    if (!ioapic_base || irq >= ioapic_max) return;
    uint32_t lo, hi;
    ioapic_read_rte(irq, &lo, &hi);
    lo &= ~IOAPIC_RTE_MASKED;
    ioapic_write_rte(irq, lo, hi);
}

/* ── Öffentlich: ioapic_get_id ───────────────────────────────────── */
uint8_t ioapic_get_id(void)
{
    if (!ioapic_base) return 0xFF;
    return (uint8_t)((ioapic_read(IOAPIC_REG_ID) >> 24) & 0x0F);
}

uint8_t ioapic_max_irqs(void) { return ioapic_max; }

/* ── 8259-PIC deaktivieren ───────────────────────────────────────── */
static void pic_disable(void)
{
    /*
     * Alle Interrupts des 8259-PICs maskieren.
     * Master-PIC (0x21) und Slave-PIC (0xA1) auf 0xFF setzen.
     * Dadurch werden keine Spurious-Interrupts an den CPU weitergeleitet.
     */
    outb(0xA1, 0xFF);
    io_wait();
    outb(0x21, 0xFF);
    io_wait();
    KINFO("I/O APIC: 8259-PIC deaktiviert\n");
}

/* ── Öffentlich: ioapic_init ─────────────────────────────────────── */
void ioapic_init(void)
{
    /* MMIO-Bereich identitätsmappen (4 KB genügen für Registersatz) */
    paging_map_page(IOAPIC_BASE_PHYS, IOAPIC_BASE_PHYS,
                    PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);
    ioapic_base = (volatile uint32_t *)IOAPIC_BASE_PHYS;

    /* Versions-Register lesen: Bits 23:16 = Max-IRQ-Eingang */
    uint32_t ver  = ioapic_read(IOAPIC_REG_VER);
    ioapic_max    = (uint8_t)(((ver >> 16) & 0xFF) + 1);
    if (ioapic_max > IOAPIC_MAX_IRQS) ioapic_max = IOAPIC_MAX_IRQS;

    uint8_t  id   = ioapic_get_id();
    KINFO("I/O APIC: ID=%u  Version=0x%02x  IRQs=%u\n",
          id, ver & 0xFF, ioapic_max);

    /* Alle IRQ-Eingänge zunächst maskieren */
    for (uint8_t i = 0; i < ioapic_max; i++) {
        ioapic_write_rte(i,
            /* Vektor: 0x20+i (Standardzuordnung), maskiert */
            (uint32_t)(0x20 + i) | IOAPIC_RTE_MASKED,
            0u  /* Ziel: APIC-ID 0 (BSP) */
        );
    }

    /* Standard-IRQ-Routing einrichten: IRQ0–IRQ15 → Vektor 0x20–0x2F */
    for (uint8_t irq = 0; irq < 16 && irq < ioapic_max; irq++) {
        uint32_t flags = (uint32_t)IOAPIC_DELMODE_FIXED << 8;
        /* IRQ0 (PIT) und IRQ8 (RTC) als flankengetriggert, aktiv-hoch */
        ioapic_set_redir(irq, (uint8_t)(0x20 + irq), 0, flags);
        /* Demaskieren */
        ioapic_unmask(irq);
    }

    /* 8259-PIC deaktivieren, damit I/O APIC alleinige Kontrolle hat */
    pic_disable();

    KINFO("I/O APIC: IRQ-Routing konfiguriert (IRQ0–IRQ%u)\n",
          (ioapic_max > 16 ? 15 : ioapic_max - 1));
}

/* ── Öffentlich: ioapic_dump ─────────────────────────────────────── */
void ioapic_dump(void)
{
    if (!ioapic_base) { KINFO("I/O APIC: nicht initialisiert\n"); return; }
    KINFO("I/O APIC-Redirect-Tabelle (erste 16 Einträge):\n");
    for (uint8_t i = 0; i < 16 && i < ioapic_max; i++) {
        uint32_t lo, hi;
        ioapic_read_rte(i, &lo, &hi);
        KINFO("  IRQ%02u → Vektor=0x%02x  Maskiert=%u  Ziel=APIC%u\n",
              i, lo & 0xFF,
              !!(lo & IOAPIC_RTE_MASKED),
              (hi >> IOAPIC_RTE_DEST_SHIFT) & 0xFF);
    }
}