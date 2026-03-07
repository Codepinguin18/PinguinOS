/**
 * @file heap.h
 * @brief Kernel heap allocator (linked-list first-fit) for PinguinOS.
 *
 * Provides kmalloc / kfree / krealloc similar to the user-space malloc
 * family, operating on virtual kernel-space memory backed by the
 * physical page allocator.
 *
 * The heap grows upward from KHEAP_START in PAGE_SIZE increments
 * as needed (lazy expansion).
 */
#ifndef _HEAP_H
#define _HEAP_H

#include "types.h"

/* ── Heap address space ──────────────────────────────────────────── */
#define KHEAP_START   0xD0000000   /* Virtual start of the kernel heap */
#define KHEAP_MAX     0xE0000000   /* Hard upper limit (256 MB heap)   */

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Initialise the kernel heap.
 *
 * Must be called after paging_init() and pmm_init().
 * Maps the first few pages of the heap area.
 */
void heap_init(void);

/**
 * @brief Allocate @p size bytes from the kernel heap.
 * @return Pointer to the allocated block, or NULL on failure.
 *
 * Blocks are always at least 8-byte aligned.
 */
void *kmalloc(size_t size);

/**
 * @brief Allocate @p size bytes, zero-initialised.
 */
void *kzalloc(size_t size);

/**
 * @brief Allocate @p size bytes aligned to @p align bytes.
 * @param align  Must be a power of two.
 */
void *kmalloc_aligned(size_t size, size_t align);

/**
 * @brief Free a block previously returned by kmalloc / kzalloc.
 * @param ptr  Pointer to free; a NULL pointer is silently ignored.
 */
void kfree(void *ptr);

/**
 * @brief Resize an allocated block.
 * @param ptr   Existing allocation (may be NULL for a fresh alloc).
 * @param size  New size in bytes.
 * @return Pointer to the resized block, or NULL on failure.
 *
 * On failure the original block is not freed.
 */
void *krealloc(void *ptr, size_t size);

/** @return Total bytes currently allocated on the kernel heap. */
size_t heap_used(void);

/** @return Total bytes available on the kernel heap. */
size_t heap_available(void);

#endif /* _HEAP_H */
