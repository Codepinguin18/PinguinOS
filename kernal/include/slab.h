/**
 * @file slab.h
 * @brief SLAB-Allocator für kleine Kernel-Objekte (Feature #1).
 *
 * Der SLAB-Allocator verwaltet Caches für häufig allozierte Objekte
 * gleicher Größe (z.B. task_t, vma_t, …). Jeder Cache hat mehrere
 * „Slabs" (zusammenhängende Seiten), die in Slots der Cache-Objektgröße
 * unterteilt sind. Freie Slots werden in einer Freelist verkettet.
 *
 *  Vorteile gegenüber generischem kmalloc:
 *   – Keine interne Fragmentierung für Objekte gleicher Größe.
 *   – Cache-freundlich: Objekte liegen dicht beieinander.
 *   – Schnelle Allokation O(1) bei nicht-leerer Freelist.
 */
#ifndef _SLAB_H
#define _SLAB_H

#include "types.h"

/* ── Konstanten ────────────────────────────────────────────────── */
#define SLAB_NAME_LEN   32
#define SLAB_MAX_CACHES 64

/* ── Slab-Deskriptor (ein Block aufeinanderfolgender Seiten) ───── */
typedef struct slab {
    void         *base;          /* Virtuelle Basisadresse des Slab    */
    uint32_t      total;         /* Gesamtzahl der Objekte im Slab     */
    uint32_t      free_count;    /* Anzahl freier Slots                */
    void        **freelist;      /* Zeiger auf ersten freien Slot      */
    struct slab  *next;          /* Nächster Slab im Cache             */
} slab_t;

/* ── Cache-Deskriptor (verwaltet Slabs einer bestimmten Objektgröße) */
typedef struct slab_cache {
    char          name[SLAB_NAME_LEN];
    size_t        obj_size;      /* Größe eines Objekts in Bytes       */
    size_t        obj_align;     /* Ausrichtung (mind. 8)              */
    uint32_t      objs_per_slab; /* Objekte pro Slab (berechnet)       */
    uint32_t      slab_pages;    /* Seiten pro Slab                    */
    slab_t       *partial;       /* Slabs mit freien Slots             */
    slab_t       *full;          /* Voll belegte Slabs                 */
    slab_t       *empty;         /* Leere (komplett freie) Slabs       */
    uint64_t      alloc_count;   /* Statistik: Allokationen            */
    uint64_t      free_count;    /* Statistik: Freigaben               */
} slab_cache_t;

/* ── Öffentliche API ────────────────────────────────────────────── */

/** Initialisiert das SLAB-Subsystem. Muss nach heap_init() aufgerufen werden. */
void slab_init(void);

/**
 * @brief Erstellt einen neuen Objekt-Cache.
 * @param name      Bezeichner (für Debugging).
 * @param obj_size  Größe eines Objekts in Bytes.
 * @param align     Ausrichtung (0 = automatisch).
 * @return Zeiger auf den neuen Cache, oder NULL bei Fehler.
 */
slab_cache_t *slab_cache_create(const char *name, size_t obj_size, size_t align);

/**
 * @brief Zerstört einen Cache und gibt alle Slabs frei.
 * @param cache  Zu zerstörender Cache. Muss vollständig frei sein.
 */
void slab_cache_destroy(slab_cache_t *cache);

/**
 * @brief Alloziert ein Objekt aus dem Cache.
 * @return Zeiger auf das Objekt, oder NULL wenn kein Speicher.
 */
void *slab_alloc(slab_cache_t *cache);

/**
 * @brief Gibt ein Objekt zurück in den Cache.
 * @param cache  Cache, aus dem das Objekt stammt.
 * @param obj    Zeiger auf das freizugebende Objekt.
 */
void slab_free(slab_cache_t *cache, void *obj);

/** Gibt Statistiken aller Caches auf dem seriellen Port aus. */
void slab_dump(void);

/* ── Vordefinierte Kernel-Caches (werden in slab_init angelegt) ── */
extern slab_cache_t *slab_cache_task;   /* sizeof(task_t)  */
extern slab_cache_t *slab_cache_vma;    /* sizeof(vma_t)   */
extern slab_cache_t *slab_cache_inode;  /* sizeof(inode_t) */

#endif /* _SLAB_H */
