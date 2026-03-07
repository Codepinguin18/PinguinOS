/**
 * @file idt.h
 * @brief Interrupt Descriptor Table und IRQ-Schnittstelle für PinguinOS.
 *
 * Die IDT verknüpft CPU-Exception-Vektoren (0–31) und Hardware-IRQ-Vektoren
 * (32–47 nach PIC-Remapping) mit Handler-Funktionen.
 *
 * IRQ-Remapping:
 *   PIC1 (Master) → IRQ  0–7  gemappt auf Vektoren 32–39
 *   PIC2 (Slave)  → IRQ  8–15 gemappt auf Vektoren 40–47
 */
#ifndef _IDT_H
#define _IDT_H

#include "types.h"

/* ── Vektornummern ───────────────────────────────────────────────── */
#define IDT_ENTRIES      256
#define IRQ_BASE         32     /* Erster Hardware-IRQ-Vektor          */
#define IRQ_TIMER        0      /* PIT Kanal 0                         */
#define IRQ_KEYBOARD     1
#define IRQ_CASCADE      2      /* PIC2 Kaskade (nicht maskieren)      */
#define IRQ_COM2         3
#define IRQ_COM1         4
#define IRQ_LPT2         5
#define IRQ_FLOPPY       6
#define IRQ_LPT1         7
#define IRQ_RTC          8
#define IRQ_MOUSE        12
#define IRQ_FPU          13
#define IRQ_ATA1         14
#define IRQ_ATA2         15

/* ── Gate-Typ / Attribut-Flags ───────────────────────────────────── */
#define IDT_FLAG_PRESENT   (1 << 7)
#define IDT_FLAG_RING0     (0 << 5)
#define IDT_FLAG_RING3     (3 << 5)
#define IDT_GATE_INTERRUPT 0x0E    /* 32-Bit Interrupt-Gate             */
#define IDT_GATE_TRAP      0x0F    /* 32-Bit Trap-Gate                  */

/* ── PIC I/O Ports ───────────────────────────────────────────────── */
#define PIC1_CMD   0x20
#define PIC1_DATA  0x21
#define PIC2_CMD   0xA0
#define PIC2_DATA  0xA1
#define PIC_EOI    0x20   /* End-of-Interrupt Befehl                   */

/* ── Roher IDT Gate Deskriptor (8 Bytes) ─────────────────────────── */
typedef struct idt_entry {
    uint16_t offset_low;   /* Bits  0–15 der Handler-Adresse          */
    uint16_t selector;     /* Code-Segment-Selektor                   */
    uint8_t  zero;         /* Immer 0                                 */
    uint8_t  type_attr;    /* Typ + Attribut-Flags                    */
    uint16_t offset_high;  /* Bits 16–31 der Handler-Adresse          */
} PACKED idt_entry_t;

/* ── IDT-Deskriptor (geladen via lidt) ───────────────────────────── */
typedef struct idt_descriptor {
    uint16_t size;
    uint32_t offset;
} PACKED idt_descriptor_t;

/**
 * @brief CPU Register Snapshot, gepusht von den ISR-Stubs.
 *
 * Das Layout muss exakt der Push-Reihenfolge in isr.S entsprechen.
 */
typedef struct cpu_regs {
    /* Segment-Register */
    uint32_t gs, fs, es, ds;
    /* Mehrzweck-Register (gepusht via pushad) */
    uint32_t edi, esi, ebp, esp_dummy;
    uint32_t ebx, edx, ecx, eax;
    /* ISR Metadaten */
    uint32_t int_no;     /* Interrupt- / Exception-Nummer             */
    uint32_t err_code;   /* Fehlercode (0 wenn keiner vorhanden)     */
    /* CPU-gepusht beim Interrupt-Einstieg */
    uint32_t eip, cs, eflags;
    uint32_t esp, ss;    /* Nur bei Privilegstufen-Wechsel vorhanden  */
} cpu_regs_t;

/** Signatur eines High-Level Interrupt-Handlers. */
typedef void (*irq_handler_t)(cpu_regs_t *regs);

/* ── Öffentliche API ─────────────────────────────────────────────── */
/**
 * @brief Initialisiert und lädt die IDT, remappt den PIC.
 */
void idt_init(void);

/**
 * @brief Registriert einen Handler für eine Hardware-IRQ-Leitung (0–15).
 * @param irq      Hardware-IRQ-Nummer (0–15).
 * @param handler  Funktion, die bei diesem IRQ aufgerufen werden soll.
 */
void irq_register(uint8_t irq, irq_handler_t handler);

/**
 * @brief Hebt die Registrierung des Handlers für eine Hardware-IRQ-Leitung auf.
 * @param irq  Hardware-IRQ-Nummer (0–15).
 */
void irq_unregister(uint8_t irq);

/**
 * @brief Registriert einen Handler für eine CPU-Exception (0–31).
 * @param vec      Vektornummer der Exception (0–31).
 * @param handler  Aufzurufende Funktion.
 */
void exception_register(uint8_t vec, irq_handler_t handler);

/**
 * @brief Maskiert (deaktiviert) eine Hardware-IRQ-Leitung.
 * @param irq  Hardware-IRQ-Nummer (0–15).
 */
void irq_mask(uint8_t irq);

/**
 * @brief Demaskiert (aktiviert) eine Hardware-IRQ-Leitung.
 * @param irq  Hardware-IRQ-Nummer (0–15).
 */
void irq_unmask(uint8_t irq);

/** Sendet End-of-Interrupt an den/die PIC(s). */
void pic_send_eoi(uint8_t irq);

#endif /* _IDT_H */
