/**
 * @file gdt.h
 * @brief Global Descriptor Table (GDT) Schnittstelle für PinguinOS.
 *
 * Die GDT definiert Speichersegmente für die CPU. Wir verwenden ein
 * Flat-Memory-Modell mit einem minimalen Satz an Deskriptoren:
 *
 *   Index  Selektor   Beschreibung
 *   ─────  ────────  ─────────────────────────
 *     0    0x00      Null-Deskriptor (erforderlich)
 *     1    0x08      Kernel-Code  (Ring 0, exec/read)
 *     2    0x10      Kernel-Daten (Ring 0, read/write)
 *     3    0x18      User-Code    (Ring 3, exec/read)
 *     4    0x20      User-Daten   (Ring 3, read/write)
 *     5    0x28      TSS          (Task State Segment)
 */
#ifndef _GDT_H
#define _GDT_H

#include "types.h"

/* ── Segment-Selektoren ──────────────────────────────────────────── */
#define GDT_KERNEL_CODE  0x08
#define GDT_KERNEL_DATA  0x10
#define GDT_USER_CODE    0x18
#define GDT_USER_DATA    0x20
#define GDT_TSS_SEG      0x28

#define GDT_ENTRIES      6

/* ── Access-Byte Bits ────────────────────────────────────────────── */
#define GDT_ACCESS_PRESENT   (1 << 7)   /* Segment ist im Speicher präsent */
#define GDT_ACCESS_RING0     (0 << 5)   /* Deskriptor-Privilegstufe 0    */
#define GDT_ACCESS_RING3     (3 << 5)   /* Deskriptor-Privilegstufe 3    */
#define GDT_ACCESS_SYSTEM    (0 << 4)   /* System-Segment (TSS, LDT …)   */
#define GDT_ACCESS_CODE_DATA (1 << 4)   /* Code/Daten-Segment            */
#define GDT_ACCESS_EXEC      (1 << 3)   /* Ausführbares (Code) Segment   */
#define GDT_ACCESS_DC        (1 << 2)   /* Direction/Conforming-Bit      */
#define GDT_ACCESS_RW        (1 << 1)   /* Lesbar (Code) / Schreibbar (Daten) */
#define GDT_ACCESS_ACCESSED  (1 << 0)   /* CPU setzt dies beim Zugriff   */

/* ── Flags-Nibble ────────────────────────────────────────────────── */
#define GDT_FLAG_GRANULARITY (1 << 3)   /* Limit in 4KB-Seiten           */
#define GDT_FLAG_SIZE        (1 << 2)   /* 32-Bit Protected Mode Segment */
#define GDT_FLAG_LONG        (1 << 1)   /* 64-Bit Code-Segment           */

/* ── Roher GDT-Eintrag (8 Bytes) ─────────────────────────────────── */
typedef struct gdt_entry {
    uint16_t limit_low;    /* Bits  0–15 des Limits                   */
    uint16_t base_low;     /* Bits  0–15 der Basisadresse             */
    uint8_t  base_mid;     /* Bits 16–23 der Basisadresse             */
    uint8_t  access;       /* Zugriff-Byte                            */
    uint8_t  limit_flags;  /* Obere 4 Bits Limit | untere 4 Bits Flags */
    uint8_t  base_high;    /* Bits 24–31 der Basisadresse             */
} PACKED gdt_entry_t;

/* ── GDT-Deskriptor (geladen via lgdt) ───────────────────────────── */
typedef struct gdt_descriptor {
    uint16_t size;         /* Größe der GDT in Bytes minus 1          */
    uint32_t offset;       /* Lineare Adresse der GDT                 */
} PACKED gdt_descriptor_t;

/* ── Task State Segment (minimal, für Ring-3 → Ring-0 Übergänge) ─── */
typedef struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;         /* Kernel-Stackpointer für Ring-0           */
    uint32_t ss0;          /* Kernel-Stacksegment                      */
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

/* ── Öffentliche API ─────────────────────────────────────────────── */
/**
 * @brief Initialisiert und lädt die GDT.
 *
 * Muss sehr früh in der Kernel-Initialisierung aufgerufen werden, bevor
 * jeglicher Interrupt- oder Segmentregister-abhängiger Code läuft.
 */
void gdt_init(void);

/**
 * @brief Setzt den Kernel-Modus Stackpointer im TSS.
 * @param stack  Oberkante des neuen Kernel-Stacks (virtuelle Adresse).
 *
 * Rufen Sie dies auf, wann immer der Scheduler zu einem neuen Task wechselt,
 * damit die CPU weiß, welchen Stack sie bei einem Interrupt im User-Modus nutzen soll.
 */
void tss_set_kernel_stack(uint32_t stack);

#endif /* _GDT_H */
