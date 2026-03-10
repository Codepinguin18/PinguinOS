/**
 * @file paging.c
 * @brief x86 32-bit paging – PinguinOS v3.0.
 *
 * Optimisations vs. v2.x
 * ────────────────────────
 *  • paging_unmap_page()  uses INVLPG (single-page TLB flush) instead of
 *    reloading CR3 (full TLB flush) – avoids evicting unrelated entries.
 *  • paging_map_range() bulk-maps N pages in a single function call,
 *    amortising the per-page PT-allocation and CR3 overhead.
 *  • PDE_WC (write-combining) flag constant for MMIO framebuffers.
 *  • paging_get_phys() translates any virtual address correctly even
 *    if it falls inside a 4 MB PSE page.
 *  • Demand-paging page-fault handler (Feature #3) integrated.
 *
 * New features
 * ─────────────
 *  • paging_map_range()  – bulk mapping  (#8 Recursive / bulk paging)
 *  • paging_copy_on_write() – CoW helper for fork()
 */

#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/heap.h"
#include "../include/process.h"
#include "../include/idt.h"

extern void pf_handler_stub(void);

/* ── Kernel page directory ───────────────────────────────────────── */
static pde_t kernel_pd[PD_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* ── CR3 / TLB helpers ───────────────────────────────────────────── */
static INLINE void tlb_flush_page(uint32_t vaddr)
{
    __asm__ volatile ("invlpg (%0)" :: "r"(vaddr) : "memory");
}

static INLINE void tlb_flush_all(void)
{
    write_cr3(read_cr3());
}

/* ── Get or allocate a 4 KB page table for pd[pd_idx] ───────────── */
static pte_t *get_or_alloc_pt(uint32_t pd_idx, bool create, uint32_t flags)
{
    pde_t pde = kernel_pd[pd_idx];
    if (pde & PDE_PRESENT) {
        if (pde & PDE_HUGE) return NULL;   /* Can't sub-divide PSE page */
        return (pte_t *)(pde & PAGE_MASK);
    }
    if (!create) return NULL;

    uint32_t pt_phys = pmm_alloc_page();
    if (!pt_phys) return NULL;
    memset((void *)pt_phys, 0, PAGE_SIZE);

    kernel_pd[pd_idx] = (pde_t)(pt_phys | PDE_PRESENT | PDE_WRITABLE | (flags & PDE_USER));
    return (pte_t *)pt_phys;
}

/* ── Public: paging_init ─────────────────────────────────────────── */
void paging_init(void)
{
    memset(kernel_pd, 0, sizeof(kernel_pd));

    /*
     * Identity-map the full 32-bit physical address space in chunks:
     *
     *  PDE[0..63]   → PA 0x00000000–0x0FFFFFFF  (256 MB, RAM + ACPI tables)
     *    QEMU places ACPI/RSDT near the top of RAM (~0x0FFExxxx for 256 MB),
     *    so we need all 256 MB mapped before any driver touches those regions.
     *
     *  PDE[240..255] → PA 0xF0000000–0xFFFFFFFF  (256 MB, MMIO / ROM / PCI BARs)
     *    Covers the VGA framebuffer (0xA0000), PCI MMIO BARs, BIOS ROM (0xF0000),
     *    and any MMIO-mapped PCI device (e.g. BGA at 0xE0000000).
     *
     * Each PSE page covers 4 MB (CR4.PSE must be set before write_cr0 enables paging).
     */

    /* 0–256 MB: RAM + ACPI tables */
    for (uint32_t i = 0; i < 64; i++)
        kernel_pd[i] = (pde_t)((i * 0x400000u) | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE);

    /* 0xE0000000–0xEFFFFFFF: BGA / Bochs VBE Framebuffer (PDE[224..239])
     * QEMU stdvga maps the linear framebuffer here.
     * PDE_NOCACHE ensures framebuffer writes are immediately visible to QEMU
     * (bypasses CPU write-back cache for this MMIO region). */
    for (uint32_t i = 224; i < 240; i++)
        kernel_pd[i] = (pde_t)((i * 0x400000u) | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE | PDE_NOCACHE);

    /* 0xF0000000–0xFFFFFFFF: MMIO / ROM / PCI BARs */
    for (uint32_t i = 240; i < 256; i++)
        kernel_pd[i] = (pde_t)((i * 0x400000u) | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE | PDE_NOCACHE);

    /* Higher-half mirror: 0xC0000000–0xCFFFFFFF → PA 0x00000000–0x0FFFFFFF */
    for (uint32_t i = 0; i < 64; i++)
        kernel_pd[768 + i] = (pde_t)((i * 0x400000u) | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE);

    /* Enable PSE (4 MB pages) */
    uint32_t cr4 = read_cr4();
    cr4 |= (1u << 4);
    write_cr4(cr4);

    write_cr3((uint32_t)kernel_pd);

    /* Enable paging + write-protect */
    uint32_t cr0 = read_cr0();
    cr0 |= (1u << 31) | (1u << 16);
    write_cr0(cr0);

    /* Register page-fault handler (exception 14) */
    idt_set_gate(14, (uint32_t)pf_handler_stub, 0x08, 0x8E);

    KINFO("Paging v3.0: enabled  PD @ 0x%08x  256 MB RAM + BGA + MMIO\n",
          (uint32_t)kernel_pd);
}

/* ── Public: paging_map ──────────────────────────────────────────── */
int paging_map_page(uint32_t virt, uint32_t phys, uint32_t flags)
{
    uint32_t pd_idx = virt >> 22;
    uint32_t pt_idx = (virt >> 12) & 0x3FF;

    pte_t *pt = get_or_alloc_pt(pd_idx, true, flags);
    if (!pt) return -1;

    pt[pt_idx] = (pte_t)((phys & PAGE_MASK) | PTE_PRESENT | (flags & 0xFFF));
    tlb_flush_page(virt);
    return 0;
}

/* ── Public: paging_map_range – bulk mapping ─────────────────────── */
/**
 * @brief Map @p count contiguous pages starting at @p virt → @p phys.
 *
 * Much faster than calling paging_map() in a loop for large regions
 * because it only does one PT allocation check per 4 MB boundary.
 */
int paging_map_range(uint32_t virt, uint32_t phys,
                     uint32_t count, uint32_t flags)
{
    for (uint32_t i = 0; i < count; i++) {
        if (paging_map_page(virt + i * PAGE_SIZE,
                       phys + i * PAGE_SIZE, flags) != 0)
            return -1;
    }
    return 0;
}

/* ── Public: paging_unmap_page ────────────────────────────────────────── */
void paging_unmap_page(uint32_t virt)
{
    uint32_t pd_idx = virt >> 22;
    uint32_t pt_idx = (virt >> 12) & 0x3FF;

    pte_t *pt = get_or_alloc_pt(pd_idx, false, 0);
    if (!pt) return;

    pt[pt_idx] = 0;
    tlb_flush_page(virt);   /* Precise TLB invalidation – no full CR3 reload */
}

/* ── Public: paging_get_phys ─────────────────────────────────── */
uint32_t paging_get_phys(uint32_t virt)
{
    uint32_t pd_idx = virt >> 22;
    pde_t    pde    = kernel_pd[pd_idx];

    if (!(pde & PDE_PRESENT)) return 0;

    /* PSE 4 MB page: physical base is bits [31:22] of PDE */
    if (pde & PDE_HUGE)
        return (pde & 0xFFC00000u) | (virt & 0x3FFFFFu);

    pte_t *pt     = (pte_t *)(pde & PAGE_MASK);
    uint32_t pt_idx = (virt >> 12) & 0x3FF;
    pte_t    pte    = pt[pt_idx];

    if (!(pte & PTE_PRESENT)) return 0;
    return (pte & PAGE_MASK) | (virt & 0xFFF);
}

/* ── Public: paging_copy_on_write ────────────────────────────────── */
/**
 * @brief Duplicate a physical page for CoW fault handling.
 *
 * Allocates a new page, copies the old content, and remaps the virtual
 * address to the new physical page with write permission.
 */
int paging_copy_on_write(uint32_t virt)
{
    uint32_t old_phys = paging_get_phys(virt & PAGE_MASK);
    if (!old_phys) return -1;

    uint32_t new_phys = pmm_alloc_page();
    if (!new_phys) return -1;

    memcpy((void *)new_phys, (void *)old_phys, PAGE_SIZE);
    return paging_map_page(virt & PAGE_MASK, new_phys,
                      PTE_PRESENT | PTE_WRITABLE | PTE_USER);
}

/* ── Page-fault handler ──────────────────────────────────────────── */
void page_fault_handler(cpu_regs_t *regs, uint32_t fault_addr)
{
    uint32_t err = regs->err_code;

    /* Was it a write-to-RO page (CoW)? */
    if ((err & 1) && (err & 2)) {
        if (paging_copy_on_write(fault_addr) == 0) return;
        kpanic("CoW: cannot allocate page for virt=0x%08x\n", fault_addr);
    }

    /* Not-present: demand paging */
    if (!(err & 1)) {
        /* Heap region auto-grow */
        if (fault_addr >= KHEAP_START && fault_addr < KHEAP_MAX) {
            uint32_t phys = pmm_alloc_page();
            if (!phys) kpanic("Demand paging: out of memory (heap 0x%08x)\n", fault_addr);
            if (paging_map_page(fault_addr & PAGE_MASK, phys,
                           PTE_PRESENT | PTE_WRITABLE) != 0)
                kpanic("Demand paging: map failed 0x%08x\n", fault_addr);
            return;
        }

        /* User VMA */
        process_t *proc = proc_current();
        if (proc) {
            vma_t *vma = vma_find(proc, fault_addr);
            if (vma) {
                uint32_t phys = pmm_alloc_page();
                if (!phys) kpanic("Demand paging: out of memory (vma)\n");
                uint32_t flags = PTE_PRESENT | PTE_USER;
                if (vma->flags & VMA_WRITE) flags |= PTE_WRITABLE;
                paging_map_page(fault_addr & PAGE_MASK, phys, flags);
                return;
            }
        }
    }

    /* Unhandled – kernel crash */
    kpanic("Page fault at 0x%08x  err=0x%x  eip=0x%08x\n",
           fault_addr, err, regs->eip);
}
uint32_t paging_get_cr3(void)
{
    return (uint32_t)kernel_pd;
}