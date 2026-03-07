/**
 * @file gdt.h
 * @brief Global Descriptor Table (GDT) interface for PinguinOS.
 *
 * The GDT defines memory segments for the CPU.  We use a flat-memory model
 * with a minimal set of descriptors:
 *
 *   Index  Selector  Description
 *   ─────  ────────  ─────────────────────────
 *     0    0x00      Null descriptor (required)
 *     1    0x08      Kernel code  (ring 0, exec/read)
 *     2    0x10      Kernel data  (ring 0, read/write)
 *     3    0x18      User   code  (ring 3, exec/read)
 *     4    0x20      User   data  (ring 3, read/write)
 *     5    0x28      TSS          (task state segment)
 */
#ifndef _GDT_H
#define _GDT_H

#include "types.h"

/* ── Segment selectors ───────────────────────────────────────────── */
#define GDT_KERNEL_CODE  0x08
#define GDT_KERNEL_DATA  0x10
#define GDT_USER_CODE    0x18
#define GDT_USER_DATA    0x20
#define GDT_TSS_SEG      0x28

#define GDT_ENTRIES      6

/* ── Access byte bits ────────────────────────────────────────────── */
#define GDT_ACCESS_PRESENT   (1 << 7)   /* Segment is present in memory   */
#define GDT_ACCESS_RING0     (0 << 5)   /* Descriptor privilege level 0   */
#define GDT_ACCESS_RING3     (3 << 5)   /* Descriptor privilege level 3   */
#define GDT_ACCESS_SYSTEM    (0 << 4)   /* System segment (TSS, LDT …)    */
#define GDT_ACCESS_CODE_DATA (1 << 4)   /* Code/data segment              */
#define GDT_ACCESS_EXEC      (1 << 3)   /* Executable (code) segment      */
#define GDT_ACCESS_DC        (1 << 2)   /* Direction/conforming bit       */
#define GDT_ACCESS_RW        (1 << 1)   /* Readable (code) / writable (data) */
#define GDT_ACCESS_ACCESSED  (1 << 0)   /* CPU sets this on access        */

/* ── Flags nibble ────────────────────────────────────────────────── */
#define GDT_FLAG_GRANULARITY (1 << 3)   /* Limit in 4KB pages            */
#define GDT_FLAG_SIZE        (1 << 2)   /* 32-bit protected mode segment  */
#define GDT_FLAG_LONG        (1 << 1)   /* 64-bit code segment            */

/* ── Raw GDT entry (8 bytes) ─────────────────────────────────────── */
typedef struct gdt_entry {
    uint16_t limit_low;    /* Bits  0–15 of the limit                 */
    uint16_t base_low;     /* Bits  0–15 of the base address          */
    uint8_t  base_mid;     /* Bits 16–23 of the base address          */
    uint8_t  access;       /* Access byte                             */
    uint8_t  limit_flags;  /* Upper 4 bits limit | lower 4 bits flags */
    uint8_t  base_high;    /* Bits 24–31 of the base address          */
} PACKED gdt_entry_t;

/* ── GDT descriptor (loaded via lgdt) ───────────────────────────── */
typedef struct gdt_descriptor {
    uint16_t size;         /* Size of GDT in bytes minus 1            */
    uint32_t offset;       /* Linear address of the GDT               */
} PACKED gdt_descriptor_t;

/* ── Task State Segment (minimal, for ring-3 → ring-0 transitions) ─ */
typedef struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;         /* Kernel stack pointer for ring-0          */
    uint32_t ss0;          /* Kernel stack segment                     */
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip, eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap_base;
} PACKED tss_entry_t;

/* ── Public API ──────────────────────────────────────────────────── */
/**
 * @brief Initialise and load the GDT.
 *
 * Must be called very early in kernel initialisation, before any
 * interrupt or segment-register dependent code.
 */
void gdt_init(void);

/**
 * @brief Set the kernel-mode stack pointer in the TSS.
 * @param stack  Top of the new kernel stack (virtual address).
 *
 * Call this whenever the scheduler switches to a new task so that the CPU
 * knows which stack to use when an interrupt occurs in user mode.
 */
void tss_set_kernel_stack(uint32_t stack);

#endif /* _GDT_H */
