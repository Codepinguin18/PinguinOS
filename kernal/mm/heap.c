/**
 * @file heap.c
 * @brief First-fit linked-list kernel heap allocator for PinguinOS.
 *
 * Blocks are prefixed with a heap_block_t header.  Free blocks are kept
 * in a singly-linked free list.  Adjacent free blocks are coalesced on
 * kfree() to reduce fragmentation.
 *
 * When the allocator needs more space it maps new pages from the physical
 * page allocator starting at KHEAP_START.
 */

#include "../include/heap.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Block header ────────────────────────────────────────────────── */
#define HEAP_MAGIC_FREE  0xDEADBEEF
#define HEAP_MAGIC_USED  0xBEEFCAFE
#define HEAP_MIN_SPLIT   32   /* Minimum leftover to split off         */

typedef struct heap_block {
    uint32_t          magic;
    size_t            size;     /* Payload size (not including header) */
    bool              free;
    struct heap_block *next;    /* Next block in heap (address order)  */
} heap_block_t;

/* ── Module state ────────────────────────────────────────────────── */
static heap_block_t *heap_head   = NULL;   /* First block              */
static uint32_t      heap_brk    = 0;      /* Next unmapped virtual addr */
static size_t        used_bytes  = 0;
static size_t        total_bytes = 0;

/* ── Map more pages into the heap area ───────────────────────────── */
static bool heap_grow(size_t needed)
{
    /* Round up to whole pages */
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

/* ── Public: heap_init ───────────────────────────────────────────── */
void heap_init(void)
{
    heap_brk = KHEAP_START;

    /* Map the initial 4 pages (16 KB) */
    if (!heap_grow(4 * PAGE_SIZE)) {
        kpanic("heap_init: cannot map initial heap pages\n");
        return;
    }

    /* Create one large free block covering all mapped pages */
    heap_head = (heap_block_t *)KHEAP_START;
    heap_head->magic = HEAP_MAGIC_FREE;
    heap_head->size  = total_bytes - sizeof(heap_block_t);
    heap_head->free  = true;
    heap_head->next  = NULL;

    KINFO("Heap: initialised at 0x%08x  (%u KB)\n",
          KHEAP_START, total_bytes / 1024);
}

/* ── Internal: split a block if it's large enough ───────────────── */
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

/* ── Public: kmalloc ─────────────────────────────────────────────── */
void *kmalloc(size_t size)
{
    if (!size) return NULL;

    /* Align payload to 8 bytes */
    size = ALIGN_UP(size, 8);

    /* First-fit search */
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

    /* No suitable block – expand the heap */
    size_t grow_size = sizeof(heap_block_t) + size;
    if (grow_size < PAGE_SIZE) grow_size = PAGE_SIZE;

    uint32_t old_brk = heap_brk;
    if (!heap_grow(grow_size)) return NULL;

    /* Carve a new block from the newly mapped pages */
    heap_block_t *new_blk = (heap_block_t *)old_brk;
    new_blk->magic = HEAP_MAGIC_FREE;
    new_blk->size  = (heap_brk - old_brk) - sizeof(heap_block_t);
    new_blk->free  = true;
    new_blk->next  = NULL;

    /* Append to the list */
    if (!heap_head) {
        heap_head = new_blk;
    } else {
        heap_block_t *last = heap_head;
        while (last->next) last = last->next;
        last->next = new_blk;
    }

    /* Retry the allocation */
    return kmalloc(size - (size - size));   /* tail-call with same size */
}

/* ── Public: kzalloc ─────────────────────────────────────────────── */
void *kzalloc(size_t size)
{
    void *p = kmalloc(size);
    if (p) memset(p, 0, size);
    return p;
}

/* ── Public: kmalloc_aligned ─────────────────────────────────────── */
void *kmalloc_aligned(size_t size, size_t align)
{
    /* Simple over-allocate and align manually.
     * We store the original pointer just before the returned address. */
    void *raw = kmalloc(size + align + sizeof(void *));
    if (!raw) return NULL;

    uintptr_t addr = (uintptr_t)raw + sizeof(void *);
    addr = ALIGN_UP(addr, align);

    ((void **)addr)[-1] = raw;
    return (void *)addr;
}

/* ── Public: kfree ───────────────────────────────────────────────── */
void kfree(void *ptr)
{
    if (!ptr) return;

    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));

    if (blk->magic != HEAP_MAGIC_USED) {
        kpanic("kfree: bad magic 0x%08x at %p\n", blk->magic, ptr);
        return;
    }

    blk->free  = true;
    blk->magic = HEAP_MAGIC_FREE;
    used_bytes -= blk->size;

    /* Coalesce adjacent free blocks */
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

/* ── Public: krealloc ────────────────────────────────────────────── */
void *krealloc(void *ptr, size_t size)
{
    if (!ptr)  return kmalloc(size);
    if (!size) { kfree(ptr); return NULL; }

    heap_block_t *blk = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (blk->size >= size) return ptr;   /* Existing block is big enough */

    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, blk->size);
    kfree(ptr);
    return new_ptr;
}

/* ── Public: statistics ──────────────────────────────────────────── */
size_t heap_used(void)      { return used_bytes;  }
size_t heap_available(void) { return total_bytes - used_bytes; }
