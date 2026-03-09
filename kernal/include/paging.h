/**
 * @file paging.h
 * @brief x86 32-bit paging (two-level page tables) for PinguinOS.
 *
 * We use a simple identity-mapped kernel space (virtual == physical for
 * the first 4 MB) plus a higher-half mapping of the entire kernel.
 *
 * Layout:
 *   0x00000000 – 0x003FFFFF  Identity-mapped (kernel bootstrap)
 *   0xC0000000 – 0xFFFFFFFF  Higher-half kernel window
 *
 * Each process will eventually get its own page directory; the kernel
 * page directory entries are shared across all processes.
 */
#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/* ── Page directory / table entry flags ──────────────────────────── */
#define PDE_PRESENT    (1 << 0)   /* Entry is valid                   */
#define PDE_WRITABLE   (1 << 1)   /* Region is writable               */
#define PDE_USER       (1 << 2)   /* Accessible from user mode        */
#define PDE_WRITETHRU  (1 << 3)   /* Write-through caching            */
#define PDE_NOCACHE    (1 << 4)   /* Cache disabled                   */
#define PDE_ACCESSED   (1 << 5)   /* CPU set on access                */
#define PDE_DIRTY      (1 << 6)   /* CPU set on write (PTEs only)     */
#define PDE_HUGE       (1 << 7)   /* 4 MB page (PDEs only)            */
#define PDE_GLOBAL     (1 << 8)   /* Don't flush from TLB on cr3 load */

/* PTE alias for clarity */
#define PTE_PRESENT    PDE_PRESENT
#define PTE_WRITABLE   PDE_WRITABLE
#define PTE_USER       PDE_USER

/* Number of entries in a page directory / page table */
#define PD_ENTRIES   1024
#define PT_ENTRIES   1024

/* Extract the page directory index from a virtual address */
#define PD_INDEX(va)   (((uint32_t)(va)) >> 22)
/* Extract the page table index from a virtual address */
#define PT_INDEX(va)   ((((uint32_t)(va)) >> 12) & 0x3FF)
/* Extract the byte offset within a page */
#define PAGE_OFFSET(va) (((uint32_t)(va)) & 0xFFF)

/* ── Types ───────────────────────────────────────────────────────── */
typedef uint32_t pde_t;   /* Page directory entry */
typedef uint32_t pte_t;   /* Page table entry     */

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Initialise paging and enable the CR0.PG bit.
 *
 * Sets up the kernel page directory with:
 *  – an identity map of the first 4 MB (so the code executing right
 *    after enabling paging still works),
 *  – a 4 MB mapping at the higher-half kernel base.
 *
 * After this call virtual addresses 0xC0000000 + offset map to the
 * same physical memory as 0x00000000 + offset.
 */
void paging_init(void);

/**
 * @brief Map a single virtual page to a physical page in the kernel PD.
 * @param virt   Virtual address (page-aligned).
 * @param phys   Physical address (page-aligned).
 * @param flags  Combination of PDE_* flags.
 * @return 0 on success, -1 on failure.
 */
int paging_map_page(uint32_t virt, uint32_t phys, uint32_t flags);

/**
 * @brief Duplicate a physical page for CoW fault handling.
 */
int paging_copy_on_write(uint32_t virt);

/**
 * @brief Unmap a single virtual page.
 * @param virt  Virtual address (page-aligned).
 */
void paging_unmap_page(uint32_t virt);

/**
 * @brief Flush the entire TLB by reloading CR3.
 */
void paging_flush_tlb(void);

/**
 * @brief Invalidate a single TLB entry for the given virtual address.
 */
void paging_invalidate(uint32_t virt);

/**
 * @brief Return the physical address mapped to @p virt in the kernel PD,
 *        or 0 if not mapped.
 */
uint32_t paging_get_phys(uint32_t virt);

/** @return Physical address of the kernel page directory. */
uint32_t paging_get_cr3(void);

#endif /* _PAGING_H */
