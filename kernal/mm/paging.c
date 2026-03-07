/**
 * @file paging.c
 * @brief x86 32-bit two-level paging implementation for PinguinOS.
 *
 * Feature #3 – Demand Paging:
 *   A page-fault handler (exception 14) is registered during paging_init().
 *   When a fault occurs for an address covered by a VMA of the current
 *   process a fresh physical page is allocated and mapped on the fly.
 *   Faults outside any VMA cause a kernel panic (Segmentation Fault).
 */

#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/heap.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/idt.h"
#include "../include/process.h"

/* ── Kernel page directory (statically allocated, page-aligned) ─── */
static pde_t kernel_pd[PD_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

/* ── Feature #3: Demand Paging state ────────────────────────────── */
static bool demand_paging_active = false;

/* ── Feature #3: Page-Fault Handler ─────────────────────────────── */
/**
 * The CPU pushes the faulting linear address into CR2 before calling
 * exception 14.  Error-code bits:
 *   bit 0: 0=non-present, 1=protection violation
 *   bit 1: 0=read,        1=write
 *   bit 2: 0=kernel,      1=user
 */
static void page_fault_handler(cpu_regs_t *regs)
{
    uint32_t fault_addr = read_cr2();
    uint32_t err        = regs->err_code;

    /* Protection violations (present page, wrong permissions) are always
     * fatal – we only handle non-present faults. */
    if (err & 1) {
        kpanic("Page-Fault: protection violation @ 0x%08x  err=0x%x  eip=0x%08x\n",
               fault_addr, err, regs->eip);
    }

    if (!demand_paging_active) {
        kpanic("Page-Fault: paging not ready @ 0x%08x  eip=0x%08x\n",
               fault_addr, regs->eip);
    }

    /* Heap region – just map a zeroed page */
    if (fault_addr >= KHEAP_START && fault_addr < KHEAP_MAX) {
        uint32_t virt  = ALIGN_DOWN(fault_addr, PAGE_SIZE);
        uint32_t phys  = pmm_alloc_page();
        if (!phys)
            kpanic("Demand-Paging: out of memory @ 0x%08x\n", fault_addr);

        memset((void *)phys, 0, PAGE_SIZE);
        paging_map(virt, phys, PDE_PRESENT | PDE_WRITABLE);
        return;
    }

    /* User-space fault – check VMA list of the current process */
    process_t *proc = proc_current();
    if (proc) {
        vma_t *vma = vma_find(proc, fault_addr);
        if (vma) {
            /* Check write permission */
            if ((err & 2) && !(vma->flags & VMA_WRITE)) {
                kpanic("Demand-Paging: write to read-only VMA @ 0x%08x\n",
                       fault_addr);
            }

            uint32_t virt  = ALIGN_DOWN(fault_addr, PAGE_SIZE);
            uint32_t phys  = pmm_alloc_page();
            if (!phys)
                kpanic("Demand-Paging: out of memory @ 0x%08x\n", fault_addr);

            memset((void *)phys, 0, PAGE_SIZE);

            uint32_t pflags = PDE_PRESENT | PDE_USER;
            if (vma->flags & VMA_WRITE) pflags |= PDE_WRITABLE;

            paging_map(virt, phys, pflags);

            KINFO("Demand-Paging: mapped 0x%08x → phys 0x%08x (pid %u)\n",
                  virt, phys, proc->pid);
            return;
        }
    }

    kpanic("Segmentation Fault @ 0x%08x  err=0x%x  eip=0x%08x\n",
           fault_addr, err, regs->eip);
}

/* ── Public: paging_init ─────────────────────────────────────────── */
void paging_init(void)
{
    memset(kernel_pd, 0, sizeof(kernel_pd));

    /* Identity map first 4 MB (PSE) */
    kernel_pd[0] = (pde_t)(0x00000000 | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE);

    /* Higher-half: 0xC0000000 → 0x00000000 */
    kernel_pd[768] = (pde_t)(0x00000000 | PDE_PRESENT | PDE_WRITABLE | PDE_HUGE);

    /* Enable PSE */
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 4);
    __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));

    write_cr3((uint32_t)kernel_pd);

    uint32_t cr0 = read_cr0();
    cr0 |= (1U << 31) | (1U << 16);
    write_cr0(cr0);

    /* Register demand-paging page-fault handler (exception #14) */
    exception_register(14, page_fault_handler);
    demand_paging_active = true;

    KINFO("Paging: enabled  (PD @ 0x%08x, demand paging ON)\n",
          (uint32_t)kernel_pd);
}

/* ── Internal: get or allocate a page table ──────────────────────── */
static pte_t *get_page_table(uint32_t pd_idx, bool create, uint32_t flags)
{
    if (kernel_pd[pd_idx] & PDE_PRESENT) {
        if (kernel_pd[pd_idx] & PDE_HUGE) return NULL;
        return (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    }
    if (!create) return NULL;

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

    uint32_t pd_idx  = PD_INDEX(virt);
    uint32_t pt_idx  = PT_INDEX(virt);
    uint32_t pd_flags = (flags & (PDE_WRITABLE | PDE_USER)) | PDE_PRESENT;

    pte_t *pt = get_page_table(pd_idx, true, pd_flags);
    if (!pt) kpanic("paging_map: OOM mapping 0x%08x\n", virt);

    pt[pt_idx] = (pte_t)(phys | flags | PDE_PRESENT);
    paging_invalidate(virt);
}

/* ── Public: paging_unmap ────────────────────────────────────────── */
void paging_unmap(uint32_t virt)
{
    virt &= PAGE_MASK;
    pte_t *pt = get_page_table(PD_INDEX(virt), false, 0);
    if (!pt) return;
    pt[PT_INDEX(virt)] = 0;
    paging_invalidate(virt);
}

/* ── Public: paging_flush_tlb / paging_invalidate ───────────────── */
void paging_flush_tlb(void)   { write_cr3(read_cr3()); }
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

    if (kernel_pd[pd_idx] & PDE_HUGE)
        return (kernel_pd[pd_idx] & 0xFFC00000) | (virt & 0x003FFFFF);

    pte_t *pt = (pte_t *)(kernel_pd[pd_idx] & PAGE_MASK);
    if (!(pt[pt_idx] & PDE_PRESENT)) return 0;

    return (pt[pt_idx] & PAGE_MASK) | PAGE_OFFSET(virt);
}

/* ── Public: paging_get_cr3 ──────────────────────────────────────── */
uint32_t paging_get_cr3(void) { return (uint32_t)kernel_pd; }