/**
 * @file mm.h
 * @brief Physical memory manager (bitmap allocator) for PinguinOS.
 *
 * The physical memory manager tracks 4 KB pages using a flat bitmap.
 * Each bit represents one page:  0 = free, 1 = used/reserved.
 *
 * Design:
 *   – The bitmap itself is placed just after the kernel BSS section.
 *   – Pages in the bitmap range cover physical addresses 0 – (max_pages*4KB).
 *   – On boot, all pages are initially marked used; the multiboot memory
 *     map is walked to mark free (available) regions, then the kernel
 *     image and bitmap are re-marked as used.
 */
#ifndef _MM_H
#define _MM_H

#include "types.h"
#include "multiboot.h"

/* ── Page size constants ─────────────────────────────────────────── */
#define PAGE_SIZE        4096U
#define PAGE_SHIFT       12
#define PAGE_MASK        (~(PAGE_SIZE - 1))

/* Convert between byte addresses and page frame numbers (PFNs). */
#define ADDR_TO_PFN(a)   ((uint32_t)(a) >> PAGE_SHIFT)
#define PFN_TO_ADDR(p)   ((uint32_t)(p) << PAGE_SHIFT)

/* Maximum amount of physical RAM we manage (1 GB default). */
#define PHYS_MEM_MAX     (1024U * 1024U * 1024U)
#define MAX_PAGES        (PHYS_MEM_MAX / PAGE_SIZE)   /* 262144 pages */

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Initialise the physical memory manager.
 * @param mbi  Multiboot information structure from the bootloader.
 *
 * This function:
 *  1. Calculates total available RAM from the BIOS memory map.
 *  2. Places the page bitmap right after the kernel's BSS.
 *  3. Marks all pages as reserved, then sets free pages according to
 *     the multiboot memory map.
 *  4. Re-marks the kernel + bitmap region as used.
 */
void pmm_init(multiboot_info_t *mbi);

/**
 * @brief Allocate one physical page (4 KB, page-aligned).
 * @return Physical address of the allocated page, or 0 on failure.
 */
uint32_t pmm_alloc_page(void);

/**
 * @brief Free a previously allocated physical page.
 * @param addr  Physical address of the page (must be page-aligned).
 */
void pmm_free_page(uint32_t addr);

/**
 * @brief Allocate a contiguous run of @p count physical pages.
 * @param count  Number of contiguous pages required.
 * @return Physical address of the first page, or 0 on failure.
 */
uint32_t pmm_alloc_pages(uint32_t count);

/**
 * @brief Free a contiguous run of @p count pages starting at @p addr.
 */
void pmm_free_pages(uint32_t addr, uint32_t count);

/** @return Total number of free pages. */
uint32_t pmm_free_page_count(void);

/** @return Total number of pages managed. */
uint32_t pmm_total_page_count(void);

/**
 * @brief Mark a physical page range as used (reserve it).
 * @param start  Start address (rounded down to page boundary).
 * @param end    End   address (rounded up   to page boundary).
 */
void pmm_mark_used(uint32_t start, uint32_t end);

/**
 * @brief Mark a physical page range as free.
 */
void pmm_mark_free(uint32_t start, uint32_t end);

#endif /* _MM_H */
