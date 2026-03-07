/**
 * @file phys_alloc.c
 * @brief Bitmap-based physical page allocator for PinguinOS.
 *
 * The bitmap is placed immediately after the kernel image in memory.
 * One bit per 4 KB page:  0 = free, 1 = used/reserved.
 *
 * After pmm_init() the bitmap reflects the usable memory described by
 * the multiboot memory map, with the kernel image and bitmap itself
 * marked as used.
 */

#include "../include/mm.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Kernel image boundaries (exported by linker.ld) ─────────────── */
extern uint32_t _kernel_start;   /* Physical start of the kernel */
extern uint32_t _kernel_end;     /* Physical end   of the kernel */

/* ── Module-private state ────────────────────────────────────────── */

/* The bitmap lives right after the kernel in physical memory.
 * MAX_PAGES / 8 = 32768 bytes = 32 KB for 1 GB of RAM tracked. */
static uint32_t *bitmap       = NULL;
static uint32_t  total_pages  = 0;
static uint32_t  free_pages   = 0;
static uint32_t  bitmap_pages = 0;   /* Pages occupied by the bitmap */

/* ── Bitmap helpers ──────────────────────────────────────────────── */

static INLINE void bitmap_set(uint32_t pfn)
{
    bitmap[pfn / 32] |=  (1U << (pfn % 32));
}

static INLINE void bitmap_clear(uint32_t pfn)
{
    bitmap[pfn / 32] &= ~(1U << (pfn % 32));
}

static INLINE bool bitmap_test(uint32_t pfn)
{
    return (bitmap[pfn / 32] >> (pfn % 32)) & 1U;
}

/* ── Public: pmm_init ────────────────────────────────────────────── */
void pmm_init(multiboot_info_t *mbi)
{
    /*
     * Step 1 – determine total memory.
     * Use the simple mem_upper field first (KB above 1 MB), extended
     * to the first byte of the memory map if available.
     */
    uint32_t mem_bytes = 0;

    if (mbi->flags & MULTIBOOT_INFO_MMAP) {
        /* Walk the memory map to find the highest available byte */
        uint32_t offset = 0;
        while (offset < mbi->mmap_length) {
            multiboot_mmap_entry_t *entry =
                (multiboot_mmap_entry_t *)(mbi->mmap_addr + offset);

            uint64_t end = entry->addr + entry->len;
            if (end > mem_bytes && entry->type == MULTIBOOT_MMAP_AVAILABLE)
                mem_bytes = (uint32_t)((end > 0xFFFFFFFFULL) ? 0xFFFFFFFF : end);

            offset += entry->size + 4;
        }
    }

    if (!mem_bytes) {
        /* Fall back: mem_upper is KB of extended memory starting at 1 MB */
        mem_bytes = (1024 + mbi->mem_upper) * 1024;
    }

    /* Cap at our hard limit */
    if (mem_bytes > PHYS_MEM_MAX)
        mem_bytes = PHYS_MEM_MAX;

    total_pages = ADDR_TO_PFN(ALIGN_UP(mem_bytes, PAGE_SIZE));
    free_pages  = 0;

    /*
     * Step 2 – place the bitmap right after the kernel.
     * _kernel_end is page-aligned by the linker script.
     */
    bitmap = (uint32_t *)((uintptr_t)&_kernel_end);

    uint32_t bitmap_bytes = (total_pages + 7) / 8;   /* round up to bytes */
    bitmap_bytes = ALIGN_UP(bitmap_bytes, PAGE_SIZE);
    bitmap_pages = bitmap_bytes / PAGE_SIZE;

    /* Mark everything as used initially */
    memset(bitmap, 0xFF, bitmap_bytes);

    /*
     * Step 3 – walk the memory map and free available regions.
     */
    if (mbi->flags & MULTIBOOT_INFO_MMAP) {
        uint32_t offset = 0;
        while (offset < mbi->mmap_length) {
            multiboot_mmap_entry_t *entry =
                (multiboot_mmap_entry_t *)(mbi->mmap_addr + offset);

            if (entry->type == MULTIBOOT_MMAP_AVAILABLE) {
                /* Clamp to 32-bit and page-align inward */
                uint32_t start = (uint32_t)ALIGN_UP(
                    (uint64_t)entry->addr > 0xFFFFFFFF ? 0xFFFFFFFF : (uint32_t)entry->addr,
                    PAGE_SIZE);
                uint64_t end64 = entry->addr + entry->len;
                uint32_t end = (uint32_t)ALIGN_DOWN(
                    end64 > 0xFFFFFFFFULL ? 0xFFFFFFFF : (uint32_t)end64,
                    PAGE_SIZE);

                if (end > start)
                    pmm_mark_free(start, end);
            }

            offset += entry->size + 4;
        }
    } else {
        /* No memory map: trust mem_lower / mem_upper */
        /* Lower memory (0–640 KB) */
        pmm_mark_free(0x1000, mbi->mem_lower * 1024);
        /* Extended memory (1 MB onwards) */
        pmm_mark_free(0x100000, mem_bytes);
    }

    /*
     * Step 4 – re-mark kernel + bitmap as used so we never give
     * those pages away.
     */
    uint32_t kern_start = (uint32_t)&_kernel_start;
    uint32_t kern_end   = (uint32_t)&_kernel_end;
    uint32_t bmap_end   = kern_end + bitmap_bytes;

    /* Always protect page 0 (real-mode IVT) */
    pmm_mark_used(0, PAGE_SIZE);

    /* Kernel image */
    pmm_mark_used(ALIGN_DOWN(kern_start, PAGE_SIZE),
                  ALIGN_UP(kern_end,     PAGE_SIZE));

    /* Bitmap */
    pmm_mark_used(ALIGN_DOWN(kern_end,  PAGE_SIZE),
                  ALIGN_UP(bmap_end,    PAGE_SIZE));

    KINFO("PMM: %u MB total  |  %u KB bitmap  |  %u pages free\n",
          mem_bytes / (1024 * 1024),
          bitmap_bytes / 1024,
          free_pages);
}

/* ── Public: pmm_mark_used / pmm_mark_free ───────────────────────── */
void pmm_mark_used(uint32_t start, uint32_t end)
{
    uint32_t pfn_start = ADDR_TO_PFN(start);
    uint32_t pfn_end   = ADDR_TO_PFN(end);

    for (uint32_t pfn = pfn_start; pfn < pfn_end && pfn < total_pages; pfn++) {
        if (!bitmap_test(pfn)) {
            bitmap_set(pfn);
            if (free_pages > 0) free_pages--;
        }
    }
}

void pmm_mark_free(uint32_t start, uint32_t end)
{
    uint32_t pfn_start = ADDR_TO_PFN(ALIGN_UP(start,   PAGE_SIZE));
    uint32_t pfn_end   = ADDR_TO_PFN(ALIGN_DOWN(end,   PAGE_SIZE));

    for (uint32_t pfn = pfn_start; pfn < pfn_end && pfn < total_pages; pfn++) {
        if (bitmap_test(pfn)) {
            bitmap_clear(pfn);
            free_pages++;
        }
    }
}

/* ── Public: pmm_alloc_page ──────────────────────────────────────── */
uint32_t pmm_alloc_page(void)
{
    /* Start search from page 1 (skip page 0) */
    for (uint32_t pfn = 1; pfn < total_pages; pfn++) {
        if (!bitmap_test(pfn)) {
            bitmap_set(pfn);
            free_pages--;
            uint32_t addr = PFN_TO_ADDR(pfn);
            memset((void *)addr, 0, PAGE_SIZE);   /* Zero the page */
            return addr;
        }
    }
    return 0;   /* Out of memory */
}

/* ── Public: pmm_free_page ───────────────────────────────────────── */
void pmm_free_page(uint32_t addr)
{
    uint32_t pfn = ADDR_TO_PFN(addr);
    if (pfn == 0 || pfn >= total_pages) return;
    if (bitmap_test(pfn)) {
        bitmap_clear(pfn);
        free_pages++;
    }
}

/* ── Public: contiguous alloc/free ──────────────────────────────── */
uint32_t pmm_alloc_pages(uint32_t count)
{
    if (count == 0) return 0;
    if (count == 1) return pmm_alloc_page();

    /* Find a contiguous run of @count free pages */
    uint32_t run_start = 0, run_len = 0;
    for (uint32_t pfn = 1; pfn < total_pages; pfn++) {
        if (!bitmap_test(pfn)) {
            if (run_len == 0) run_start = pfn;
            run_len++;
            if (run_len == count) {
                /* Mark all as used */
                for (uint32_t i = run_start; i < run_start + count; i++) {
                    bitmap_set(i);
                    free_pages--;
                }
                uint32_t addr = PFN_TO_ADDR(run_start);
                memset((void *)addr, 0, count * PAGE_SIZE);
                return addr;
            }
        } else {
            run_len = 0;
        }
    }
    return 0;
}

void pmm_free_pages(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
        pmm_free_page(addr + i * PAGE_SIZE);
}

/* ── Public: statistics ──────────────────────────────────────────── */
uint32_t pmm_free_page_count(void)  { return free_pages;  }
uint32_t pmm_total_page_count(void) { return total_pages; }
