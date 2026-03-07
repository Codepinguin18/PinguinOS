/**
 * @file paging.c
 * @brief x86 32-Bit Zweistufige Paging Implementierung für PinguinOS.
 *
 * Wir verwenden 4 KB Pages und eine zweistufige Page-Table (PD + PT).
 * Das Kernel-Page-Directory ist statisch im BSS allokiert.
 * Page-Tables, die für neue Mappings benötigt werden, werden über pmm_alloc_page() allokiert.
 *
 * Nach paging_init() ist die MMU aktiv mit:
 *   – Identity Map: 0x00000000 → PA 0x00000000 (erste 4 MB, 4 MB Page)
 *   – Kernel Map:   0xC0000000 → PA 0x00000000 (erste 4 MB, 4 MB Page)
 */

#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Kernel-Page-Directory (statisch allokiert, Page-aligned) ────── */
static pde_t kernel_pd[PD_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* ── Öffentlich: paging_init ─────────────────────────────────────── */
void paging_init(void)
{
    memset(kernel_pd, 0, sizeof(kernel_pd));

    /*
     * Die ersten 4 MB identitätsmappen (Identity Map) mittels einer
     * 4 MB (PSE) Page, damit die CPU sofort nach dem Setzen von CR0.PG
     * korrekt weiterarbeitet.
     *
     * Der Eintrag verwendet PDE_HUGE (PS Bit), sodass wir keine
     * Page-Table benötigen.
     */
    kernel_pd[0] = (pde_t)(
        0x00000000 |
        PDE_PRESENT | PDE_WRITABLE | PDE_HUGE
    );

    /*
     * Higher-Half Kernel Mapping: Virtuell 0xC0000000 → Physisch 0x00000000.
     * PD-Index für 0xC0000000 = 0xC0000000 >> 22 = 768.
     */
    kernel_pd[768] = (pde_t)(
        0x00000000 |
        PDE_PRESENT | PDE_WRITABLE | PDE_HUGE
    );

    /*
     * PSE (4 MB Pages) in CR4 aktivieren, bevor CR3 geladen wird.
     */
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 4);   /* CR4.PSE = 1 */
    __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));

    /* Unser Page-Directory in CR3 laden */
    write_cr3((uint32_t)kernel_pd);

    /* Paging aktivieren: CR0.PG (Bit 31) und CR0.WP (Bit 16) setzen */
    uint32_t cr0 = read_cr0();
    cr0 |= (1U << 31) | (1U << 16);
    write_cr0(cr0);

    KINFO("Paging: aktiviert  (PD @ 0x%08x)\n", (uint32_t)kernel_pd);
}

/* ── Intern: Page-Table für einen PD-Eintrag holen oder allokieren ─ */
static pte_t *get_page_table(uint32_t pd_idx, bool create, uint32_t flags)
{
    if (kernel_pd[pd_idx] & PDE_PRESENT) {
        if (kernel_pd[pd_idx] & PDE_HUGE) {
            /* Eine Huge Page kann hier nicht unterteilt werden */
            return NULL;
        }
        return (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    }

    if (!create) return NULL;

    /* Eine neue Page-Table allokieren */
    uint32_t pt_phys = pmm_alloc_page();
    if (!pt_phys) return NULL;

    memset((void *)pt_phys, 0, PAGE_SIZE);
    kernel_pd[pd_idx] = (pde_t)(pt_phys | flags | PDE_PRESENT);

    return (pte_t *)pt_phys;
}

/* ── Öffentlich: paging_map ──────────────────────────────────────── */
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags)
{
    virt &= PAGE_MASK;
    phys &= PAGE_MASK;

    uint32_t pd_idx = PD_INDEX(virt);
    uint32_t pt_idx = PT_INDEX(virt);

    uint32_t pd_flags = (flags & (PDE_WRITABLE | PDE_USER)) | PDE_PRESENT;
    pte_t *pt = get_page_table(pd_idx, true, pd_flags);

    if (!pt) {
        kpanic("paging_map: Speicher voll beim Mapping von 0x%08x\n", virt);
        return;
    }

    pt[pt_idx] = (pte_t)(phys | flags | PDE_PRESENT);
    paging_invalidate(virt);
}

/* ── Öffentlich: paging_unmap ────────────────────────────────────── */
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

/* ── Öffentlich: paging_flush_tlb ────────────────────────────────── */
void paging_flush_tlb(void)
{
    write_cr3(read_cr3());
}

/* ── Öffentlich: paging_invalidate ───────────────────────────────── */
void paging_invalidate(uint32_t virt)
{
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

/* ── Öffentlich: paging_get_phys ─────────────────────────────────── */
uint32_t paging_get_phys(uint32_t virt)
{
    uint32_t pd_idx = PD_INDEX(virt);
    uint32_t pt_idx = PT_INDEX(virt);

    if (!(kernel_pd[pd_idx] & PDE_PRESENT)) return 0;

    if (kernel_pd[pd_idx] & PDE_HUGE) {
        /* 4 MB Page: Basis sind Bits 22–31, untere 22 Bits von virt addieren */
        return (kernel_pd[pd_idx] & 0xFFC00000) | (virt & 0x003FFFFF);
    }

    pte_t *pt = (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    if (!(pt[pt_idx] & PDE_PRESENT)) return 0;

    return (pt[pt_idx] & PAGE_MASK) | PAGE_OFFSET(virt);
}

/* ── Öffentlich: paging_get_cr3 ──────────────────────────────────── */
uint32_t paging_get_cr3(void)
{
    return (uint32_t)kernel_pd;
}
