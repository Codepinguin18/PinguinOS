/**
 * @file heap.c
 * @brief First-Fit Linked-List Kernel-Heap-Allocator für PinguinOS.
 *
 * Blöcke sind mit einem heap_block_t Header versehen. Freie Blöcke werden in
 * einer einfach verketteten Freiliste geführt. Angrenzende freie Blöcke werden
 * bei kfree() verschmolzen (Coalescing), um Fragmentierung zu reduzieren.
 *
 * Feature #94 – Memory-Leak-Detektor:
 *   Wenn HEAP_LEAK_DETECT definiert ist, speichert der Allocator Datei und Zeile
 *   für jede aktive Allokation in einer Tabelle fester Größe.
 *   heap_leak_report() gibt alle Einträge aus, die zum Zeitpunkt des Aufrufs noch vorhanden sind.
 */

#include "../include/heap.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Block-Header ────────────────────────────────────────────────── */
#define HEAP_MAGIC_FREE  0xDEADBEEF
#define HEAP_MAGIC_USED  0xBEEFCAFE
#define HEAP_MIN_SPLIT   32

typedef struct heap_block {
    uint32_t          magic;
    size_t            size;
    bool              free;
    struct heap_block *next;
} heap_block_t;

/* ── Modul-Zustand ───────────────────────────────────────────────── */
static heap_block_t *heap_head   = NULL;
static uint32_t      heap_brk    = 0;
static size_t        used_bytes  = 0;
static size_t        total_bytes = 0;

/* ── Feature #94: Leak-Detection-Tabelle ─────────────────────────── */
#define LEAK_TABLE_SIZE  1024

typedef struct alloc_record {
    void       *ptr;
    size_t      size;
    const char *file;
    int         line;
} alloc_record_t;

static alloc_record_t leak_table[LEAK_TABLE_SIZE];
static uint32_t       leak_count = 0;   /* Nächster freier Slot (mit Umlauf) */

static void leak_record(void *ptr, size_t size, const char *file, int line)
{
    /* Zuerst einen freien (ptr==NULL) Slot suchen; sonst zirkulären Index verwenden */
    for (int i = 0; i < LEAK_TABLE_SIZE; i++) {
        if (!leak_table[i].ptr) {
            leak_table[i].ptr  = ptr;
            leak_table[i].size = size;
            leak_table[i].file = file;
            leak_table[i].line = line;
            return;
        }
    }
    /* Tabelle voll – zirkulären Slot verwenden (ältester Eintrag wird überschrieben) */
    int idx = (int)(leak_count % LEAK_TABLE_SIZE);
    leak_table[idx].ptr  = ptr;
    leak_table[idx].size = size;
    leak_table[idx].file = file;
    leak_table[idx].line = line;
    leak_count++;
}

static void leak_unrecord(void *ptr)
{
    for (int i = 0; i < LEAK_TABLE_SIZE; i++) {
        if (leak_table[i].ptr == ptr) {
            leak_table[i].ptr  = NULL;
            leak_table[i].file = NULL;
            leak_table[i].line = 0;
            leak_table[i].size = 0;
            return;
        }
    }
}

/* ── Weitere Seiten in den Heap-Bereich mappen ───────────────────── */
static bool heap_grow(size_t needed)
{
    size_t pages = (needed + PAGE_SIZE - 1) / PAGE_SIZE;
    for (size_t i = 0; i < pages; i++) {
        if (heap_brk + PAGE_SIZE > KHEAP_MAX) return false;
        uint32_t phys = pmm_alloc_page();
        if (!phys) return false;
        paging_map_page(heap_brk, phys, PDE_PRESENT | PDE_WRITABLE);
        heap_brk    += PAGE_SIZE;
        total_bytes += PAGE_SIZE;
    }
    return true;
}

/* ── Öffentlich: heap_init ───────────────────────────────────────── */
void heap_init(void)
{
    heap_brk = KHEAP_START;
    memset(leak_table, 0, sizeof(leak_table));

    if (!heap_grow(4 * PAGE_SIZE))
        kpanic("heap_init: initiale Heap-Seiten konnten nicht gemappt werden\n");

    heap_head        = (heap_block_t *)KHEAP_START;
    heap_head->magic = HEAP_MAGIC_FREE;
    heap_head->size  = total_bytes - sizeof(heap_block_t);
    heap_head->free  = true;
    heap_head->next  = NULL;

    KINFO("Heap: initialisiert bei 0x%08x  (%u KB)\n",
          KHEAP_START, (uint32_t)(total_bytes / 1024));
}

/* ── Intern: Block splitten, falls der Rest nutzbar ist ─────────── */
static void heap_split(heap_block_t *blk, size_t size)
{
    if (blk->size < size + sizeof(heap_block_t) + HEAP_MIN_SPLIT) return;

    heap_block_t *new_blk = (heap_block_t *)((uint8_t *)blk
                             + sizeof(heap_block_t) + size);
    new_blk->magic = HEAP_MAGIC_FREE;
    new_blk->size  = blk->size - size - sizeof(heap_block_t);
    new_blk->free  = true;
    new_blk->next  = blk->next;

    blk->size = size;
    blk->next = new_blk;
}

/* ── Intern: Kern-Allocator (ohne Tracking) ─────────────────────── */
static void *heap_alloc_raw(size_t size)
{
    if (!size) return NULL;
    size = ALIGN_UP(size, 8);

    heap_block_t *blk = heap_head;
    while (blk) {
        if (blk->free && blk->size >= size) {
            heap_split(blk, size);
            blk->free  = false;
            blk->magic = HEAP_MAGIC_USED;
            used_bytes += blk->size;
            return (void *)((uint8_t *)blk + sizeof(heap_block_t));
        }
        blk = blk->next;
    }

    size_t grow_size = sizeof(heap_block_t) + size;
    if (grow_size < PAGE_SIZE) grow_size = PAGE_SIZE;

    uint32_t old_brk = heap_brk;
    if (!heap_grow(grow_size)) return NULL;

    heap_block_t *new_blk = (heap_block_t *)old_brk;
    new_blk->magic = HEAP_MAGIC_FREE;
    new_blk->size  = (heap_brk - old_brk) - sizeof(heap_block_t);
    new_blk->free  = true;
    new_blk->next  = NULL;

    if (!heap_head) {
        heap_head = new_blk;
    } else {
        heap_block_t *last = heap_head;
        while (last->next) last = last->next;
        last->next = new_blk;
    }

    return heap_alloc_raw(size);
}

/* ── Intern: Kern-Free (ohne Tracking) ──────────────────────────── */
static void heap_free_raw(void *ptr)
{
    if (!ptr) return;
    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));

    if (blk->magic != HEAP_MAGIC_USED)
        kpanic("kfree: ungueltige Magic 0x%08x bei %p\n", blk->magic, ptr);

    blk->free  = true;
    blk->magic = HEAP_MAGIC_FREE;
    used_bytes -= blk->size;

    heap_block_t *cur = heap_head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += sizeof(heap_block_t) + cur->next->size;
            cur->next  = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

/* ── Öffentlich: kmalloc / kzalloc / kfree / krealloc ───────────── */
void *kmalloc(size_t size)  { return heap_alloc_raw(size); }
void *kzalloc(size_t size)
{
    void *p = heap_alloc_raw(size);
    if (p) memset(p, 0, size);
    return p;
}
void kfree(void *ptr) { heap_free_raw(ptr); }

void *kmalloc_aligned(size_t size, size_t align)
{
    void *raw = kmalloc(size + align + sizeof(void *));
    if (!raw) return NULL;
    uintptr_t addr = (uintptr_t)raw + sizeof(void *);
    addr = ALIGN_UP(addr, align);
    ((void **)addr)[-1] = raw;
    return (void *)addr;
}

void *krealloc(void *ptr, size_t size)
{
    if (!ptr)  return kmalloc(size);
    if (!size) { kfree(ptr); return NULL; }
    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (blk->size >= size) return ptr;
    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, blk->size);
    kfree(ptr);
    return new_ptr;
}

/* ── Feature #94: Getrackte Wrapper ─────────────────────────────── */
void *kmalloc_tracked(size_t size, const char *file, int line)
{
    void *p = heap_alloc_raw(size);
    if (p) leak_record(p, size, file, line);
    return p;
}

void *kzalloc_tracked(size_t size, const char *file, int line)
{
    void *p = heap_alloc_raw(size);
    if (p) {
        memset(p, 0, size);
        leak_record(p, size, file, line);
    }
    return p;
}

void kfree_tracked(void *ptr, const char *file, int line)
{
    (void)file; (void)line;
    if (!ptr) return;
    leak_unrecord(ptr);
    heap_free_raw(ptr);
}

/**
 * @briefibt alle aktiven Allokationen (potenzielle Leaks) seriell aus.
 */
void heap_leak_report(void)
{
    uint32_t leaks = 0;
    serial_puts("[LEAK REPORT] ---- begin ----\n");

    char buf[128];
    for (int i = 0; i < LEAK_TABLE_SIZE; i++) {
        if (!leak_table[i].ptr) continue;
        leaks++;
        snprintf(buf, sizeof(buf),
                 "  LEAK: %p  %u bytes  %s:%d\n",
                 leak_table[i].ptr,
                 (uint32_t)leak_table[i].size,
                 leak_table[i].file ? leak_table[i].file : "?",
                 leak_table[i].line);
        serial_puts(buf);
    }

    if (!leaks)
        serial_puts("  Keine Leaks gefunden.\n");
    else {
        snprintf(buf, sizeof(buf),
                 "[LEAK REPORT] %u potenzielle(s) Leak(s)\n", leaks);
        serial_puts(buf);
    }
    serial_puts("[LEAK REPORT] ---- end ----\n");
}

/* ── Öffentlich: Statistiken ─────────────────────────────────────── */
size_t heap_used(void)      { return used_bytes; }
size_t heap_available(void) { return total_bytes - used_bytes; }

void heap_dump(void)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[HEAP] verwendet=%u  frei=%u  gesamt=%u bytes\n",
             (uint32_t)used_bytes,
             (uint32_t)(total_bytes - used_bytes),
             (uint32_t)total_bytes);
    serial_puts(buf);
}