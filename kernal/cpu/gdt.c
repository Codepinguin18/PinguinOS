/**
 * @file gdt.c
 * @brief GDT and TSS initialisation for PinguinOS.
 *
 * We create a minimal 6-descriptor GDT:
 *   0: null, 1: kernel code, 2: kernel data,
 *   3: user code, 4: user data, 5: TSS
 *
 * The TSS is needed so that hardware interrupts raised in user mode
 * (ring 3) correctly switch to the kernel stack stored in TSS.esp0.
 */

#include "../include/gdt.h"
#include "../include/klib.h"

/* ── External asm helpers declared in boot.S ───────────────────── */
extern void gdt_flush(uint32_t gdt_ptr);
extern void tss_flush(void);

/* ── Module-private data ─────────────────────────────────────────── */
static gdt_entry_t     gdt_entries[GDT_ENTRIES];
static gdt_descriptor_t gdt_desc;
static tss_entry_t      tss;

/* ── Helper: encode a raw GDT entry ─────────────────────────────── */
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

    /* Upper nibble of limit_flags = flags; lower = bits 16-19 of limit */
    gdt_entries[idx].limit_flags =
        (uint8_t)((flags & 0x0F) << 4) |
        (uint8_t)((limit >> 16) & 0x0F);

    gdt_entries[idx].access = access;
}

/* ── TSS setup ───────────────────────────────────────────────────── */
static void tss_init(void)
{
    memset(&tss, 0, sizeof(tss));

    /*
     * ss0/esp0 will be updated by tss_set_kernel_stack() whenever the
     * scheduler switches tasks.  For now point it at a dummy location.
     */
    tss.ss0  = GDT_KERNEL_DATA;
    tss.esp0 = 0;                 /* Will be set before ring-3 tasks run */

    /* The I/O permission bitmap base points past the TSS end → no IOPB */
    tss.iomap_base = (uint16_t)sizeof(tss_entry_t);

    /* Encode TSS into GDT entry 5 */
    uint32_t tss_base  = (uint32_t)&tss;
    uint32_t tss_limit = (uint32_t)(sizeof(tss_entry_t) - 1);

    gdt_entries[5].limit_low   = (uint16_t)(tss_limit & 0xFFFF);
    gdt_entries[5].base_low    = (uint16_t)(tss_base  & 0xFFFF);
    gdt_entries[5].base_mid    = (uint8_t)((tss_base  >> 16) & 0xFF);
    gdt_entries[5].access      = 0x89;   /* Present, ring-0, 32-bit TSS */
    gdt_entries[5].limit_flags = (uint8_t)((tss_limit >> 16) & 0x0F);
    gdt_entries[5].base_high   = (uint8_t)((tss_base  >> 24) & 0xFF);
}

/* ── Public: gdt_init ────────────────────────────────────────────── */
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

    /* 32-bit protected mode, 4 KB granularity */
    uint8_t flags32 = GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE;

    /* 0: null descriptor */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* 1: kernel code – base=0, limit=4 GB */
    gdt_set_entry(1, 0x00000000, 0xFFFFFFFF, kcode_acc, flags32);

    /* 2: kernel data – base=0, limit=4 GB */
    gdt_set_entry(2, 0x00000000, 0xFFFFFFFF, kdata_acc, flags32);

    /* 3: user code – base=0, limit=4 GB */
    gdt_set_entry(3, 0x00000000, 0xFFFFFFFF, ucode_acc, flags32);

    /* 4: user data – base=0, limit=4 GB */
    gdt_set_entry(4, 0x00000000, 0xFFFFFFFF, udata_acc, flags32);

    /* 5: TSS */
    tss_init();

    /* Fill the descriptor and load */
    gdt_desc.size   = (uint16_t)(sizeof(gdt_entries) - 1);
    gdt_desc.offset = (uint32_t)&gdt_entries;

    gdt_flush((uint32_t)&gdt_desc);
    tss_flush();
}

/* ── Public: tss_set_kernel_stack ────────────────────────────────── */
void tss_set_kernel_stack(uint32_t stack)
{
    tss.esp0 = stack;
}
