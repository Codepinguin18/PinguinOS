/**
 * @file idt.h
 * @brief Interrupt Descriptor Table and IRQ interface for PinguinOS.
 *
 * The IDT maps CPU exception vectors (0–31) and hardware IRQ vectors
 * (32–47 after PIC remapping) to handler functions.
 *
 * IRQ remapping:
 *   PIC1 (master) → IRQ  0–7  mapped to vectors 32–39
 *   PIC2 (slave)  → IRQ  8–15 mapped to vectors 40–47
 */
#ifndef _IDT_H
#define _IDT_H

#include "types.h"

/* ── Vector numbers ──────────────────────────────────────────────── */
#define IDT_ENTRIES      256
#define IRQ_BASE         32     /* First hardware IRQ vector           */
#define IRQ_TIMER        0      /* PIT channel 0                       */
#define IRQ_KEYBOARD     1
#define IRQ_CASCADE      2      /* PIC2 cascade (do not mask)          */
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

/* ── Gate type / attribute flags ─────────────────────────────────── */
#define IDT_FLAG_PRESENT   (1 << 7)
#define IDT_FLAG_RING0     (0 << 5)
#define IDT_FLAG_RING3     (3 << 5)
#define IDT_GATE_INTERRUPT 0x0E    /* 32-bit interrupt gate             */
#define IDT_GATE_TRAP      0x0F    /* 32-bit trap gate                  */

/* ── PIC I/O ports ───────────────────────────────────────────────── */
#define PIC1_CMD   0x20
#define PIC1_DATA  0x21
#define PIC2_CMD   0xA0
#define PIC2_DATA  0xA1
#define PIC_EOI    0x20   /* End-of-interrupt command                  */

/* ── Raw IDT gate descriptor (8 bytes) ──────────────────────────── */
typedef struct idt_entry {
    uint16_t offset_low;   /* Bits  0–15 of handler address           */
    uint16_t selector;     /* Code segment selector                   */
    uint8_t  zero;         /* Always 0                                */
    uint8_t  type_attr;    /* Type + attribute flags                  */
    uint16_t offset_high;  /* Bits 16–31 of handler address           */
} PACKED idt_entry_t;

/* ── IDT descriptor (loaded via lidt) ───────────────────────────── */
typedef struct idt_descriptor {
    uint16_t size;
    uint32_t offset;
} PACKED idt_descriptor_t;

/**
 * @brief CPU register snapshot pushed by the ISR stubs.
 *
 * The layout must exactly match the push order in isr.S.
 */
typedef struct cpu_regs {
    /* Segment registers */
    uint32_t gs, fs, es, ds;
    /* General-purpose (pushed via pushad) */
    uint32_t edi, esi, ebp, esp_dummy;
    uint32_t ebx, edx, ecx, eax;
    /* ISR metadata */
    uint32_t int_no;     /* Interrupt / exception number              */
    uint32_t err_code;   /* Error code (0 if none)                   */
    /* CPU-pushed on interrupt entry */
    uint32_t eip, cs, eflags;
    uint32_t esp, ss;    /* Only present on privilege-level change    */
} cpu_regs_t;

/** Signature of a high-level interrupt handler. */
typedef void (*irq_handler_t)(cpu_regs_t *regs);

/* ── Public API ──────────────────────────────────────────────────── */
/**
 * @brief Initialise and load the IDT, remap the PIC.
 */
void idt_init(void);
void idt_set_gate(uint8_t vector, uint32_t base, uint16_t sel, uint8_t flags);

/**
 * @brief Register a handler for a hardware IRQ line (0–15).
 * @param irq      Hardware IRQ number (0–15).
 * @param handler  Function to call on that IRQ.
 */
void irq_register(uint8_t irq, irq_handler_t handler);

/**
 * @brief Unregister the handler for a hardware IRQ line.
 * @param irq  Hardware IRQ number (0–15).
 */
void irq_unregister(uint8_t irq);

/**
 * @brief Register a handler for a CPU exception (0–31).
 * @param vec      Exception vector number (0–31).
 * @param handler  Function to call.
 */
void exception_register(uint8_t vec, irq_handler_t handler);

/**
 * @brief Mask (disable) a hardware IRQ line.
 * @param irq  Hardware IRQ number (0–15).
 */
void irq_mask(uint8_t irq);

/**
 * @brief Unmask (enable) a hardware IRQ line.
 * @param irq  Hardware IRQ number (0–15).
 */
void irq_unmask(uint8_t irq);

/** Send end-of-interrupt to the PIC(s). */
void pic_send_eoi(uint8_t irq);

#endif /* _IDT_H */
