/**
 * @file heap.c
 * @brief First-Fit Linked-List Kernel-Heap-Allocator für PinguinOS.
 *
 * Blöcke werden durch einen heap_block_t Header eingeleitet. Freie Blöcke werden
 * in einer einfach verketteten Liste geführt. Angrenzende freie Blöcke werden
 * bei kfree() verschmolzen (Coalescing), um Fragmentierung zu reduzieren.
 *
 * Benötigt der Allocator mehr Platz, mappt er neue Seiten vom physischen
 * Page-Allocator beginnend bei KHEAP_START.
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
#define HEAP_MIN_SPLIT   32   /* Minimum-Restgröße zum Abspalten      */

typedef struct heap_block {
    uint32_t          magic;
    size_t            size;     /* Nutzlast-Größe (ohne Header)       */
    bool              free;
    struct heap_block *next;    /* Nächster Block im Heap (Adressfolge) */
} heap_block_t;

/* ── Modul-Status ────────────────────────────────────────────────── */
static heap_block_t *heap_head   = NULL;   /* Erster Block             */
static uint32_t      heap_brk    = 0;      /* Nächste ungemappte virtuelle Adr */
static size_t        used_bytes  = 0;
static size_t        total_bytes = 0;

/* ── Mehr Pages in den Heap-Bereich mappen ────────────────────────── */
static bool heap_grow(size_t needed)
{
    /* Auf ganze Seiten aufrunden */
    size_t pages = (needed + PAGE_SIZE - 1) / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++) {
        if (heap_brk + PAGE_SIZE > KHEAP_MAX) return false;

        uint32_t phys = pmm_alloc_page();
        if (!phys) return false;

        paging_map(heap_brk, phys, PDE_PRESENT | PDE_WRITABLE);
        heap_brk    += PAGE_SIZE;
        total_bytes += PAGE_SIZE;
    }
    return true;
}

/* ── Öffentlich: heap_init ───────────────────────────────────────── */
void heap_init(void)
{
    heap_brk = KHEAP_START;

    /* Initiale 4 Pages mappen (16 KB) */
    if (!heap_grow(4 * PAGE_SIZE)) {
        kpanic("heap_init: initiale Heap-Seiten konnten nicht gemappt werden\n");
        return;
    }

    /* Einen großen freien Block über alle gemappten Seiten erstellen */
    heap_head = (heap_block_t *)KHEAP_START;
    heap_head->magic = HEAP_MAGIC_FREE;
    heap_head->size  = total_bytes - sizeof(heap_block_t);
    heap_head->free  = true;
    heap_head->next  = NULL;

    KINFO("Heap: initialisiert bei 0x%08x  (%u KB)\n",
          KHEAP_START, total_bytes / 1024);
}

/* ── Intern: Block aufteilen, falls er groß genug ist ───────────── */
static void heap_split(heap_block_t *blk, size_t size)
{
    size_t remaining = blk->size - size - sizeof(heap_block_t);
    if (remaining < HEAP_MIN_SPLIT) return;

    heap_block_t *new_blk = (heap_block_t *)((uint8_t *)blk
                             + sizeof(heap_block_t) + size);
    new_blk->magic = HEAP_MAGIC_FREE;
    new_blk->size  = remaining;
    new_blk->free  = true;
    new_blk->next  = blk->next;

    blk->size = size;
    blk->next = new_blk;
}

/* ── Öffentlich: kmalloc ─────────────────────────────────────────── */
void *kmalloc(size_t size)
{
    if (!size) return NULL;

    /* Nutzlast auf 8 Bytes ausrichten */
    size = ALIGN_UP(size, 8);

    /* First-Fit Suche */
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

    /* Kein passender Block – Heap erweitern */
    size_t grow_size = sizeof(heap_block_t) + size;
    if (grow_size < PAGE_SIZE) grow_size = PAGE_SIZE;

    uint32_t old_brk = heap_brk;
    if (!heap_grow(grow_size)) return NULL;

    /* Neuen Block aus den frisch gemappten Seiten herausschneiden */
    heap_block_t *new_blk = (heap_block_t *)old_brk;
    new_blk->magic = HEAP_MAGIC_FREE;
    new_blk->size  = (heap_brk - old_brk) - sizeof(heap_block_t);
    new_blk->free  = true;
    new_blk->next  = NULL;

    /* An die Liste anhängen */
    if (!heap_head) {
        heap_head = new_blk;
    } else {
        heap_block_t *last = heap_head;
        while (last->next) last = last->next;
        last->next = new_blk;
    }

    /* Allokation erneut versuchen */
    return kmalloc(size - (size - size));   /* Tail-Call mit gleicher Größe */
}

/* ── Öffentlich: kzalloc ─────────────────────────────────────────── */
void *kzalloc(size_t size)
{
    void *p = kmalloc(size);
    if (p) memset(p, 0, size);
    return p;
}

/* ── Öffentlich: kmalloc_aligned ─────────────────────────────────── */
void *kmalloc_aligned(size_t size, size_t align)
{
    /* Einfach mehr allokieren und manuell ausrichten.
     * Der Original-Pointer wird direkt vor der zurückgegebenen Adresse gespeichert. */
    void *raw = kmalloc(size + align + sizeof(void *));
    if (!raw) return NULL;

    uintptr_t addr = (uintptr_t)raw + sizeof(void *);
    addr = ALIGN_UP(addr, align);

    ((void **)addr)[-1] = raw;
    return (void *)addr;
}

/* ── Öffentlich: kfree ───────────────────────────────────────────── */
void kfree(void *ptr)
{
    if (!ptr) return;

    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));

    if (blk->magic != HEAP_MAGIC_USED) {
        kpanic("kfree: falsche Magic 0x%08x bei %p\n", blk->magic, ptr);
        return;
    }

    blk->free  = true;
    blk->magic = HEAP_MAGIC_FREE;
    used_bytes -= blk->size;

    /* Angrenzende freie Blöcke verschmelzen */
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

/* ── Öffentlich: krealloc ────────────────────────────────────────── */
void *krealloc(void *ptr, size_t size)
{
    if (!ptr)  return kmalloc(size);
    if (!size) { kfree(ptr); return NULL; }

    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (blk->size >= size) return ptr;   /* Bestehender Block ist groß genug */

    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, blk->size);
    kfree(ptr);
    return new_ptr;
}

/* ── Öffentlich: Statistiken ─────────────────────────────────────── */
size_t heap_used(void)      { return used_bytes;  }
size_t heap_available(void) { return total_bytes - used_bytes; }
