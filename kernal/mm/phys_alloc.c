/**
 * @file phys_alloc.c
 * @brief Bitmap-basierter physischer Page-Allocator für PinguinOS.
 *
 * Die Bitmap wird direkt hinter dem Kernel-Image im Speicher platziert.
 * Ein Bit pro 4 KB Page: 0 = frei, 1 = belegt/reserviert.
 *
 * Nach pmm_init() spiegelt die Bitmap den nutzbaren Speicher wider, wie er
 * in der Multiboot-Memory-Map beschrieben ist, wobei das Kernel-Image
 * und die Bitmap selbst als belegt markiert sind.
 */

#include "../include/mm.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Kernel-Image Grenzen (exportiert durch linker.ld) ────────────── */
extern uint32_t _kernel_start;   /* Physischer Start des Kernels */
extern uint32_t _kernel_end;     /* Physisches Ende des Kernels   */

/* ── Modul-privater Status ────────────────────────────────────────── */

/* Die Bitmap liegt direkt nach dem Kernel im physischen Speicher.
 * MAX_PAGES / 8 = 32768 Bytes = 32 KB für 1 GB verwalteten RAM. */
static uint32_t *bitmap       = NULL;
static uint32_t  total_pages  = 0;
static uint32_t  free_pages   = 0;
static uint32_t  bitmap_pages = 0;   /* Von der Bitmap belegte Pages */

/* ── Bitmap-Helfer ───────────────────────────────────────────────── */

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

/* ── Öffentlich: pmm_init ────────────────────────────────────────── */
void pmm_init(multiboot_info_t *mbi)
{
    /*
     * Schritt 1 – Gesamtspeicher ermitteln.
     * Zuerst das einfache mem_upper Feld verwenden (KB über 1 MB),
     * erweitert auf das erste Byte der Memory-Map, falls verfügbar.
     */
    uint32_t mem_bytes = 0;

    if (mbi->flags & MULTIBOOT_INFO_MMAP) {
        /* Die Memory-Map durchlaufen, um das höchste verfügbare Byte zu finden */
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
        /* Ausweichlösung: mem_upper sind die KB des erweiterten Speichers ab 1 MB */
        mem_bytes = (1024 + mbi->mem_upper) * 1024;
    }

    /* Auf unser hartes Limit begrenzen */
    if (mem_bytes > PHYS_MEM_MAX)
        mem_bytes = PHYS_MEM_MAX;

    total_pages = ADDR_TO_PFN(ALIGN_UP(mem_bytes, PAGE_SIZE));
    free_pages  = 0;

    /*
     * Schritt 2 – Bitmap direkt hinter dem Kernel platzieren.
     * _kernel_end ist durch das Linker-Skript Page-aligned.
     */
    bitmap = (uint32_t *)((uintptr_t)&_kernel_end);

    uint32_t bitmap_bytes = (total_pages + 7) / 8;   /* auf Bytes aufrunden */
    bitmap_bytes = ALIGN_UP(bitmap_bytes, PAGE_SIZE);
    bitmap_pages = bitmap_bytes / PAGE_SIZE;

    /* Initial alles als belegt markieren */
    memset(bitmap, 0xFF, bitmap_bytes);

    /*
     * Schritt 3 – Memory-Map durchlaufen und verfügbare Regionen freigeben.
     */
    if (mbi->flags & MULTIBOOT_INFO_MMAP) {
        uint32_t offset = 0;
        while (offset < mbi->mmap_length) {
            multiboot_mmap_entry_t *entry =
                (multiboot_mmap_entry_t *)(mbi->mmap_addr + offset);

            if (entry->type == MULTIBOOT_MMAP_AVAILABLE) {
                /* Auf 32-Bit begrenzen und nach innen Page-alignen */
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
        /* Keine Memory-Map: mem_lower / mem_upper vertrauen */
        /* Unterer Speicher (0–640 KB) */
        pmm_mark_free(0x1000, mbi->mem_lower * 1024);
        /* Erweiterter Speicher (ab 1 MB) */
        pmm_mark_free(0x100000, mem_bytes);
    }

    /*
     * Schritt 4 – Kernel + Bitmap wieder als belegt markieren, damit
     * diese Pages niemals vergeben werden.
     */
    uint32_t kern_start = (uint32_t)&_kernel_start;
    uint32_t kern_end   = (uint32_t)&_kernel_end;
    uint32_t bmap_end   = kern_end + bitmap_bytes;

    /* Page 0 immer schützen (Real-Mode IVT) */
    pmm_mark_used(0, PAGE_SIZE);

    /* Kernel-Image */
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

/* ── Öffentlich: pmm_mark_used / pmm_mark_free ───────────────────── */
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

/* ── Öffentlich: pmm_alloc_page ──────────────────────────────────── */
uint32_t pmm_alloc_page(void)
{
    /* Suche ab Seite 1 starten (Seite 0 überspringen) */
    for (uint32_t pfn = 1; pfn < total_pages; pfn++) {
        if (!bitmap_test(pfn)) {
            bitmap_set(pfn);
            free_pages--;
            uint32_t addr = PFN_TO_ADDR(pfn);
            memset((void *)addr, 0, PAGE_SIZE);   /* Seite nullen */
            return addr;
        }
    }
    return 0;   /* Speicher voll */
}

/* ── Öffentlich: pmm_free_page ───────────────────────────────────── */
void pmm_free_page(uint32_t addr)
{
    uint32_t pfn = ADDR_TO_PFN(addr);
    if (pfn == 0 || pfn >= total_pages) return;
    if (bitmap_test(pfn)) {
        bitmap_clear(pfn);
        free_pages++;
    }
}

/* ── Öffentlich: Kontinuierliche Allokation ───────────────────────── */
uint32_t pmm_alloc_pages(uint32_t count)
{
    if (count == 0) return 0;
    if (count == 1) return pmm_alloc_page();

    /* Eine zusammenhängende Sequenz von @count freien Pages suchen */
    uint32_t run_start = 0, run_len = 0;
    for (uint32_t pfn = 1; pfn < total_pages; pfn++) {
        if (!bitmap_test(pfn)) {
            if (run_len == 0) run_start = pfn;
            run_len++;
            if (run_len == count) {
                /* Alle als belegt markieren */
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

/* ── Öffentlich: Statistiken ─────────────────────────────────────── */
uint32_t pmm_free_page_count(void)  { return free_pages;  }
uint32_t pmm_total_page_count(void) { return total_pages; }
