/**
 * @file slab.c
 * @brief SLAB-Allocator Implementierung für PinguinOS.
 */
#include "../include/slab.h"
#include "../include/mm.h"
#include "../include/paging.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── Globale Cache-Tabelle ───────────────────────────────────────── */
static slab_cache_t *cache_table[SLAB_MAX_CACHES];
static uint32_t      cache_count = 0;

/* Vordefinierte Caches */
slab_cache_t *slab_cache_task  = NULL;
slab_cache_t *slab_cache_vma   = NULL;
slab_cache_t *slab_cache_inode = NULL;

/* ── Einen neuen Slab allozieren ─────────────────────────────────── */
static slab_t *slab_new(slab_cache_t *cache)
{
    /* Slab-Deskriptor auf dem Heap */
    slab_t *slab = (slab_t *)kmalloc(sizeof(slab_t));
    if (!slab) return NULL;

    /* Physische Seiten für die Nutzdaten */
    uint32_t phys = pmm_alloc_pages(cache->slab_pages);
    if (!phys) { kfree(slab); return NULL; }

    slab->base       = (void *)phys;   /* Identity-gemappt */
    slab->total      = cache->objs_per_slab;
    slab->free_count = cache->objs_per_slab;
    slab->next       = NULL;

    /* Freelist aufbauen: jeder Slot zeigt auf den nächsten */
    uint8_t *p = (uint8_t *)slab->base;
    size_t   stride = ALIGN_UP(cache->obj_size, cache->obj_align);

    for (uint32_t i = 0; i < cache->objs_per_slab - 1; i++) {
        *(void **)(p + i * stride) = (void *)(p + (i + 1) * stride);
    }
    *(void **)(p + (cache->objs_per_slab - 1) * stride) = NULL;

    slab->freelist = (void **)slab->base;
    return slab;
}

/* ── Cache erstellen ─────────────────────────────────────────────── */
slab_cache_t *slab_cache_create(const char *name, size_t obj_size, size_t align)
{
    if (cache_count >= SLAB_MAX_CACHES) return NULL;

    /* Mindestgröße: ein Zeiger muss in den Slot passen */
    if (obj_size < sizeof(void *)) obj_size = sizeof(void *);

    slab_cache_t *cache = (slab_cache_t *)kzalloc(sizeof(slab_cache_t));
    if (!cache) return NULL;

    strncpy(cache->name, name ? name : "?", SLAB_NAME_LEN - 1);
    cache->obj_size  = obj_size;
    cache->obj_align = align ? align : 8;

    /* Objekte pro Slab berechnen (1–4 Seiten) */
    size_t stride = ALIGN_UP(obj_size, cache->obj_align);
    cache->slab_pages = 1;
    while ((cache->slab_pages * PAGE_SIZE) / stride < 8 &&
           cache->slab_pages < 4)
        cache->slab_pages++;

    cache->objs_per_slab = (uint32_t)((cache->slab_pages * PAGE_SIZE) / stride);

    cache_table[cache_count++] = cache;
    return cache;
}

void slab_cache_destroy(slab_cache_t *cache)
{
    /* Nur leere Caches können zerstört werden */
    slab_t *s = cache->empty;
    while (s) {
        slab_t *next = s->next;
        pmm_free_pages((uint32_t)s->base, cache->slab_pages);
        kfree(s);
        s = next;
    }
    kfree(cache);
}

/* ── Allozieren ──────────────────────────────────────────────────── */
void *slab_alloc(slab_cache_t *cache)
{
    uint32_t flags = irq_save();

    /* Gibt es einen partiellen Slab? */
    slab_t *slab = cache->partial;

    /* Sonst einen leeren nehmen */
    if (!slab) slab = cache->empty;

    /* Sonst neuen Slab anlegen */
    if (!slab) {
        slab = slab_new(cache);
        if (!slab) { irq_restore(flags); return NULL; }
    }

    /* Objekt aus der Freelist holen */
    void *obj = slab->freelist;
    slab->freelist = (void **)*slab->freelist;
    slab->free_count--;
    cache->alloc_count++;

    /* Slab in die richtige Liste verschieben */
    if (slab->free_count == 0) {
        /* War in partial → zu full */
        if (cache->partial == slab) cache->partial = slab->next;
        slab->next   = cache->full;
        cache->full  = slab;
    } else if (slab->free_count == slab->total - 1) {
        /* War in empty → zu partial */
        if (cache->empty == slab) cache->empty = slab->next;
        slab->next     = cache->partial;
        cache->partial = slab;
    }

    irq_restore(flags);
    memset(obj, 0, cache->obj_size);
    return obj;
}

/* ── Freigeben ───────────────────────────────────────────────────── */
void slab_free(slab_cache_t *cache, void *obj)
{
    if (!obj) return;
    uint32_t flags = irq_save();

    /* Slab finden, dem dieses Objekt gehört */
    size_t stride     = ALIGN_UP(cache->obj_size, cache->obj_align);
    size_t slab_bytes = cache->slab_pages * PAGE_SIZE;

    slab_t **lists[] = { &cache->full, &cache->partial, &cache->empty, NULL };
    slab_t *found = NULL;

    for (int li = 0; lists[li]; li++) {
        slab_t **head = lists[li];
        slab_t  *prev = NULL;
        slab_t  *s    = *head;

        while (s) {
            uintptr_t base = (uintptr_t)s->base;
            if ((uintptr_t)obj >= base &&
                (uintptr_t)obj <  base + slab_bytes) {
                /* Objekt aus der Liste herausnehmen */
                if (prev) prev->next = s->next;
                else       *head     = s->next;
                found = s;
                break;
            }
            prev = s;
            s    = s->next;
        }
        if (found) break;
    }

    if (!found) {
        irq_restore(flags);
        kpanic("slab_free: Objekt %p gehört zu keinem Cache '%s'\n",
               obj, cache->name);
        return;
    }

    /* Slot in die Freelist eintragen */
    UNUSED(stride);
    *(void **)obj       = (void *)found->freelist;
    found->freelist     = (void **)obj;
    found->free_count++;
    cache->free_count++;

    /* In die richtige Liste verschieben */
    if (found->free_count == found->total) {
        found->next   = cache->empty;
        cache->empty  = found;
    } else {
        found->next    = cache->partial;
        cache->partial = found;
    }

    irq_restore(flags);
}

/* ── Init ────────────────────────────────────────────────────────── */
void slab_init(void)
{
    memset(cache_table, 0, sizeof(cache_table));
    cache_count = 0;

    /* Vordefinierte Caches – Größen werden zur Compile-Zeit aus den
     * jeweiligen Headern bekannt sein; hier Platzhalter-Größen: */
    slab_cache_task  = slab_cache_create("task_t",  256, 8);
    slab_cache_vma   = slab_cache_create("vma_t",    64, 8);
    slab_cache_inode = slab_cache_create("inode_t", 128, 8);

    KINFO("SLAB: initialisiert (%u Caches)\n", cache_count);
}

/* ── Dump ────────────────────────────────────────────────────────── */
void slab_dump(void)
{
    KINFO("SLAB Cache-Übersicht:\n");
    for (uint32_t i = 0; i < cache_count; i++) {
        slab_cache_t *c = cache_table[i];
        if (!c) continue;
        KINFO("  %-20s  obj=%u  objs/slab=%u  alloc=%llu  free=%llu\n",
              c->name, (uint32_t)c->obj_size, c->objs_per_slab,
              c->alloc_count, c->free_count);
    }
}
