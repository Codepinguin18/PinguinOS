/**
 * @file heap.h
 * @brief API für den Kernel-Heap-Allocator von PinguinOS.
 *
 * Feature #94 – Memory-Leak-Detektor:
 *   Definiere HEAP_LEAK_DETECT vor dem Einbinden dieses Headers (oder übergib
 *   -DHEAP_LEAK_DETECT an den Compiler), um kmalloc/kfree durch
 *   getrackte Versionen zu ersetzen, die Datei und Zeile jeder Allokation speichern.
 *   Rufe heap_leak_report() beim Herunterfahren auf, um nicht freigegebene Blöcke auszugeben.
 */
#ifndef _HEAP_H
#define _HEAP_H

#include "types.h"

/* ── Heap-Adressraum ─────────────────────────────────────────────── */
#define KHEAP_START   0xD0000000u
#define KHEAP_MAX     0xDFFFFFFFu

/* ── Kern-Allocator API ──────────────────────────────────────────── */

/** Initialisiert den Kernel-Heap. Muss nach paging_init() aufgerufen werden. */
void  heap_init(void);

/** Allokiert @p size Bytes. Gibt NULL bei Fehler zurück. */
void *kmalloc(size_t size);

/** Allokiert @p size mit Null initialisierte Bytes. */
void *kzalloc(size_t size);

/** Ändert die Größe einer Allokation. */
void *krealloc(void *ptr, size_t new_size);

/** Gibt einen zuvor allokierten Block frei. */
void  kfree(void *ptr);

/** Gibt Heap-Statistiken auf der seriellen Konsole aus. */
void  heap_dump(void);

/** Gibt die Anzahl der aktuell belegten Bytes im Heap zurück. */
size_t heap_used(void);

/** Gibt die Anzahl der aktuell verfügbaren Bytes im Heap zurück. */
size_t heap_available(void);

/* ── Feature #94: Memory-Leak-Detektor ──────────────────────────── */

/**
 * @brief Getrackter Allocator – speichert Datei und Zeile für jedes kmalloc.
 *
 * Dies sind die eigentlichen Funktionen; die Makros unten leiten kmalloc/kfree
 * auf diese um, wenn HEAP_LEAK_DETECT aktiv ist.
 */
void *kmalloc_tracked(size_t size, const char *file, int line);
void *kzalloc_tracked(size_t size, const char *file, int line);
void  kfree_tracked(void *ptr, const char *file, int line);

/**
 * @brief Gibt alle Allokationen aus, die nicht freigegeben wurden.
 *
 * Rufe dies beim Herunterfahren (oder aus der Debug-Shell) auf, um Leaks zu finden.
 */
void heap_leak_report(void);

/* ── Makro-Umleitung ─────────────────────────────────────────────── */
#ifdef HEAP_LEAK_DETECT
#  undef  kmalloc
#  undef  kzalloc
#  undef  kfree
#  define kmalloc(s)      kmalloc_tracked((s), __FILE__, __LINE__)
#  define kzalloc(s)      kzalloc_tracked((s), __FILE__, __LINE__)
#  define kfree(p)        kfree_tracked((p),   __FILE__, __LINE__)
#endif /* HEAP_LEAK_DETECT */

#endif /* _HEAP_H */