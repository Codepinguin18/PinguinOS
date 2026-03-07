/**
 * @file gdt.c
 * @brief GDT- und TSS-Initialisierung für PinguinOS.
 *
 * Wir erstellen eine minimale GDT mit 6 Deskriptoren:
 *   0: Null, 1: Kernel Code, 2: Kernel Data,
 *   3: User Code, 4: User Data, 5: TSS
 *
 * Das TSS wird benötigt, damit Hardware-Interrupts im User-Mode
 * (Ring 3) korrekt zum Kernel-Stack wechseln können (gespeichert in TSS.esp0).
 */

#include "../include/gdt.h"
#include "../include/klib.h"

/* ── Externe Assembler-Helfer, deklariert in boot.S ─────────────── */
extern void gdt_flush(uint32_t gdt_ptr);
extern void tss_flush(void);

/* ── Modul-private Daten ─────────────────────────────────────────── */
static gdt_entry_t     gdt_entries[GDT_ENTRIES];
static gdt_descriptor_t gdt_desc;
static tss_entry_t      tss;

/* ── Helfer: Einen rohen GDT-Eintrag kodieren ───────────────────── */
static void gdt_set_entry(int idx,
                           uint32_t base,
                           uint32_t limit,
                           uint8_t  access,
                           uint8_t  flags)
{
    gdt_entries[idx].base_low  = (uint16_t)(base  & 0xFFFF);
    gdt_entries[idx].base_mid  = (uint8_t)((base  >> 16) & 0xFF);
    gdt_entries[idx].base_high = (uint8_t)((base  >> 24) & 0xFF);

    gdt_entries[idx].limit_low = (uint16_t)(limit & 0xFFFF);

    /* Oberes Nibble von limit_flags = flags; unteres = Bits 16-19 von limit */
    gdt_entries[idx].limit_flags =
        (uint8_t)((flags & 0x0F) << 4) |
        (uint8_t)((limit >> 16) & 0x0F);

    gdt_entries[idx].access = access;
}

/* ── TSS-Einrichtung ─────────────────────────────────────────────── */
static void tss_init(void)
{
    memset(&tss, 0, sizeof(tss));

    /*
     * ss0/esp0 werden durch tss_set_kernel_stack() aktualisiert, sobald der
     * Scheduler Tasks wechselt. Vorerst zeigen sie auf einen Platzhalter.
     */
    tss.ss0  = GDT_KERNEL_DATA;
    tss.esp0 = 0;                 /* Wird gesetzt, bevor Ring-3 Tasks laufen */

    /* Die Basis des I/O-Permission-Bitmaps zeigt hinter das TSS-Ende → kein IOPB */
    tss.iomap_base = (uint16_t)sizeof(tss_entry_t);

    /* TSS in GDT-Eintrag 5 kodieren */
    uint32_t tss_base  = (uint32_t)&tss;
    uint32_t tss_limit = (uint32_t)(sizeof(tss_entry_t) - 1);

    gdt_entries[5].limit_low   = (uint16_t)(tss_limit & 0xFFFF);
    gdt_entries[5].base_low    = (uint16_t)(tss_base  & 0xFFFF);
    gdt_entries[5].base_mid    = (uint8_t)((tss_base  >> 16) & 0xFF);
    gdt_entries[5].access      = 0x89;   /* Present, Ring-0, 32-bit TSS */
    gdt_entries[5].limit_flags = (uint8_t)((tss_limit >> 16) & 0x0F);
    gdt_entries[5].base_high   = (uint8_t)((tss_base  >> 24) & 0xFF);
}

/* ── Öffentlich: gdt_init ────────────────────────────────────────── */
void gdt_init(void)
{
    uint8_t kcode_acc = GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 |
                        GDT_ACCESS_CODE_DATA | GDT_ACCESS_EXEC |
                        GDT_ACCESS_RW;

    uint8_t kdata_acc = GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 |
                        GDT_ACCESS_CODE_DATA | GDT_ACCESS_RW;

    uint8_t ucode_acc = GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 |
                        GDT_ACCESS_CODE_DATA | GDT_ACCESS_EXEC |
                        GDT_ACCESS_RW;

    uint8_t udata_acc = GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 |
                        GDT_ACCESS_CODE_DATA | GDT_ACCESS_RW;

    /* 32-Bit Protected Mode, 4 KB Granularität */
    uint8_t flags32 = GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE;

    /* 0: Null-Deskriptor */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* 1: Kernel Code – Base=0, Limit=4 GB */
    gdt_set_entry(1, 0x00000000, 0xFFFFFFFF, kcode_acc, flags32);

    /* 2: Kernel Data – Base=0, Limit=4 GB */
    gdt_set_entry(2, 0x00000000, 0xFFFFFFFF, kdata_acc, flags32);

    /* 3: User Code – Base=0, Limit=4 GB */
    gdt_set_entry(3, 0x00000000, 0xFFFFFFFF, ucode_acc, flags32);

    /* 4: User Data – Base=0, Limit=4 GB */
    gdt_set_entry(4, 0x00000000, 0xFFFFFFFF, udata_acc, flags32);

    /* 5: TSS */
    tss_init();

    /* Deskriptor füllen und laden */
    gdt_desc.size   = (uint16_t)(sizeof(gdt_entries) - 1);
    gdt_desc.offset = (uint32_t)&gdt_entries;

    gdt_flush((uint32_t)&gdt_desc);
    tss_flush();
}

/* ── Öffentlich: tss_set_kernel_stack ────────────────────────────── */
void tss_set_kernel_stack(uint32_t stack)
{
    tss.esp0 = stack;
}
