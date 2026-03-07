/**
 * @file heap.h
 * @brief Kernel-Heap-Allocator (Linked-List First-Fit) für PinguinOS.
 *
 * Bietet kmalloc / kfree / krealloc ähnlich der malloc-Familie im User-Space,
 * agiert jedoch auf virtuellem Kernel-Speicher, der durch den physischen
 * Page-Allocator abgesichert ist.
 *
 * Der Heap wächst von KHEAP_START in PAGE_SIZE Schritten nach oben,
 * wenn Bedarf besteht (Lazy Expansion).
 */
#ifndef _HEAP_H
#define _HEAP_H

#include "types.h"

/* ── Heap-Adressraum ─────────────────────────────────────────────── */
#define KHEAP_START   0xD0000000   /* Virtueller Start des Kernel-Heaps */
#define KHEAP_MAX     0xE0000000   /* Harte Obergrenze (256 MB Heap)    */

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief Initialisiert den Kernel-Heap.
 *
 * Muss nach paging_init() und pmm_init() aufgerufen werden.
 * Mappt die ersten paar Seiten des Heap-Bereichs.
 */
void heap_init(void);

/**
 * @brief Allokiert @p size Bytes vom Kernel-Heap.
 * @return Zeiger auf den allokierten Block, oder NULL im Fehlerfall.
 *
 * Blöcke sind immer mindestens auf 8 Bytes ausgerichtet.
 */
void *kmalloc(size_t size);

/**
 * @brief Allokiert @p size Bytes, mit Nullen initialisiert.
 */
void *kzalloc(size_t size);

/**
 * @brief Allokiert @p size Bytes ausgerichtet auf @p align Bytes.
 * @param align  Muss eine Potenz von zwei sein.
 */
void *kmalloc_aligned(size_t size, size_t align);

/**
 * @brief Gibt einen zuvor durch kmalloc / kzalloc erhaltenen Block frei.
 * @param ptr  Zeiger zum Freigeben; ein NULL-Zeiger wird ignoriert.
 */
void kfree(void *ptr);

/**
 * @brief Ändert die Größe eines allokierten Blocks.
 * @param ptr   Bestehende Allokation (kann NULL für eine Neuallokation sein).
 * @param size  Neue Größe in Bytes.
 * @return Zeiger auf den veränderten Block, oder NULL im Fehlerfall.
 *
 * Im Fehlerfall wird der ursprüngliche Block nicht freigegeben.
 */
void *krealloc(void *ptr, size_t size);

/** @return Gesamtzahl der aktuell im Kernel-Heap allokierten Bytes. */
size_t heap_used(void);

/** @return Gesamtzahl der im Kernel-Heap verfügbaren Bytes. */
size_t heap_available(void);

#endif /* _HEAP_H */
