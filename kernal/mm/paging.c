/**
 * @file paging.c
 * @brief x86 32-bit two-level paging implementation for PinguinOS.
 *
 * We use 4 KB pages and a two-level page table (PD + PT).
 * The kernel page directory is statically allocated in BSS.
 * Page tables needed for new mappings are allocated via pmm_alloc_page().
 *
 * After paging_init() the MMU is active with:
 *   – Identity map: 0x00000000 → PA 0x00000000 (first 4 MB, 4 MB page)
 *   – Kernel map:   0xC0000000 → PA 0x00000000 (first 4 MB, 4 MB page)
 */

#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Kernel page directory (statically allocated, page-aligned) ─── */
static pde_t kernel_pd[PD_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* ── Public: paging_init ─────────────────────────────────────────── */
void paging_init(void)
{
    memset(kernel_pd, 0, sizeof(kernel_pd));

    /*
     * Identity map the first 4 MB using a 4 MB (PSE) page so the
     * CPU still works correctly immediately after CR0.PG is set.
     *
     * The entry uses PDE_HUGE (PS bit) so we don't need a page table.
     */
    kernel_pd[0] = (pde_t)(
        0x00000000 |
        PDE_PRESENT | PDE_WRITABLE | PDE_HUGE
    );

    /*
     * Higher-half kernel mapping: virtual 0xC0000000 → physical 0x00000000.
     * PD index for 0xC0000000 = 0xC0000000 >> 22 = 768.
     */
    kernel_pd[768] = (pde_t)(
        0x00000000 |
        PDE_PRESENT | PDE_WRITABLE | PDE_HUGE
    );

    /*
     * Enable PSE (4 MB pages) in CR4 before loading CR3.
     */
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 4);   /* CR4.PSE = 1 */
    __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));

    /* Load our page directory into CR3 */
    write_cr3((uint32_t)kernel_pd);

    /* Enable paging: set CR0.PG (bit 31) and CR0.WP (bit 16) */
    uint32_t cr0 = read_cr0();
    cr0 |= (1U << 31) | (1U << 16);
    write_cr0(cr0);

    KINFO("Paging: enabled  (PD @ 0x%08x)\n", (uint32_t)kernel_pd);
}

/* ── Internal: get or allocate a page table for a PD entry ───────── */
static pte_t *get_page_table(uint32_t pd_idx, bool create, uint32_t flags)
{
    if (kernel_pd[pd_idx] & PDE_PRESENT) {
        if (kernel_pd[pd_idx] & PDE_HUGE) {
            /* Can't subdivide a huge page here */
            return NULL;
        }
        return (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    }

    if (!create) return NULL;

    /* Allocate a new page table */
    uint32_t pt_phys = pmm_alloc_page();
    if (!pt_phys) return NULL;

    memset((void *)pt_phys, 0, PAGE_SIZE);
    kernel_pd[pd_idx] = (pde_t)(pt_phys | flags | PDE_PRESENT);

    return (pte_t *)pt_phys;
}

/* ── Public: paging_map ──────────────────────────────────────────── */
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags)
{
    virt &= PAGE_MASK;
    phys &= PAGE_MASK;

    uint32_t pd_idx = PD_INDEX(virt);
    uint32_t pt_idx = PT_INDEX(virt);

    uint32_t pd_flags = (flags & (PDE_WRITABLE | PDE_USER)) | PDE_PRESENT;
    pte_t *pt = get_page_table(pd_idx, true, pd_flags);

    if (!pt) {
        kpanic("paging_map: out of memory mapping 0x%08x\n", virt);
        return;
    }

    pt[pt_idx] = (pte_t)(phys | flags | PDE_PRESENT);
    paging_invalidate(virt);
}

/* ── Public: paging_unmap ────────────────────────────────────────── */
void paging_unmap(uint32_t virt)
{
    virt &= PAGE_MASK;

    uint32_t pd_idx = PD_INDEX(virt);
    uint32_t pt_idx = PT_INDEX(virt);

    pte_t *pt = get_page_table(pd_idx, false, 0);
    if (!pt) return;

    pt[pt_idx] = 0;
    paging_invalidate(virt);
}

/* ── Public: paging_flush_tlb ────────────────────────────────────── */
void paging_flush_tlb(void)
{
    write_cr3(read_cr3());
}

/* ── Public: paging_invalidate ───────────────────────────────────── */
void paging_invalidate(uint32_t virt)
{
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

/* ── Public: paging_get_phys ─────────────────────────────────────── */
uint32_t paging_get_phys(uint32_t virt)
{
    uint32_t pd_idx = PD_INDEX(virt);
    uint32_t pt_idx = PT_INDEX(virt);

    if (!(kernel_pd[pd_idx] & PDE_PRESENT)) return 0;

    if (kernel_pd[pd_idx] & PDE_HUGE) {
        /* 4 MB page: base is bits 22–31, add lower 22 bits of virt */
        return (kernel_pd[pd_idx] & 0xFFC00000) | (virt & 0x003FFFFF);
    }

    pte_t *pt = (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    if (!(pt[pt_idx] & PDE_PRESENT)) return 0;

    return (pt[pt_idx] & PAGE_MASK) | PAGE_OFFSET(virt);
}

/* ── Public: paging_get_cr3 ──────────────────────────────────────── */
uint32_t paging_get_cr3(void)
{
    return (uint32_t)kernel_pd;
}
