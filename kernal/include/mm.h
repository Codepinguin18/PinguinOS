/**
 * @file mm.h
 * @brief Physischer Speichermanager (Bitmap-Allocator) für PinguinOS.
 *
 * Der physische Speichermanager verfolgt 4 KB Seiten mithilfe einer flachen Bitmap.
 * Jedes Bit repräsentiert eine Seite: 0 = frei, 1 = belegt/reserviert.
 *
 * Design:
 *   – Die Bitmap selbst befindet sich direkt nach der BSS-Sektion des Kernels.
 *   – Die Seiten im Bitmap-Bereich decken physische Adressen von 0 bis (max_pages * 4KB) ab.
 *   – Beim Booten sind zunächst alle Seiten als belegt markiert; die Multiboot-Memory-Map
 *     wird durchlaufen, um freie (verfügbare) Bereiche zu markieren, danach werden das
 *     Kernel-Image und die Bitmap wieder als belegt markiert.
 */
#ifndef _MM_H
#define _MM_H

#include "types.h"
#include "multiboot.h"

/* ── Seitengrößen-Konstanten ─────────────────────────────────────── */
#define PAGE_SIZE        4096U
#define PAGE_SHIFT       12
#define PAGE_MASK        (~(PAGE_SIZE - 1))

/* Umrechnung zwischen Byte-Adressen und Page Frame Numbers (PFNs). */
#define ADDR_TO_PFN(a)   ((uint32_t)(a) >> PAGE_SHIFT)
#define PFN_TO_ADDR(p)   ((uint32_t)(p) << PAGE_SHIFT)

/* Maximale Menge an physischem RAM, den wir verwalten (Standard: 1 GB). */
#define PHYS_MEM_MAX     (1024U * 1024U * 1024U)
#define MAX_PAGES        (PHYS_MEM_MAX / PAGE_SIZE)   /* 262144 Seiten */

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief Initialisiert den physischen Speichermanager.
 * @param mbi  Multiboot-Informationsstruktur vom Bootloader.
 *
 * Diese Funktion:
 *  1. Berechnet den gesamten verfügbaren RAM aus der BIOS-Memory-Map.
 *  2. Platziert die Seiten-Bitmap direkt nach der BSS-Sektion des Kernels.
 *  3. Markiert alle Seiten als reserviert und setzt dann freie Seiten gemäß
 *     der Multiboot-Memory-Map.
 *  4. Markiert den Bereich für Kernel + Bitmap wieder als belegt.
 */
void pmm_init(multiboot_info_t *mbi);

/**
 * @brief Allokiert eine physische Seite (4 KB, seiten-ausgerichtet).
 * @return Physische Adresse der allokierten Seite, oder 0 im Fehlerfall.
 */
uint32_t pmm_alloc_page(void);

/**
 * @brief Gibt eine zuvor allokierte physische Seite frei.
 * @param addr  Physische Adresse der Seite (muss seiten-ausgerichtet sein).
 */
void pmm_free_page(uint32_t addr);

/**
 * @brief Allokiert eine zusammenhängende Folge von @p count physischen Seiten.
 * @param count  Anzahl der benötigten zusammenhängenden Seiten.
 * @return Physische Adresse der ersten Seite, oder 0 im Fehlerfall.
 */
uint32_t pmm_alloc_pages(uint32_t count);

/**
 * @brief Gibt eine zusammenhängende Folge von @p count Seiten beginnend bei @p addr frei.
 */
void pmm_free_pages(uint32_t addr, uint32_t count);

/** @return Gesamtzahl der freien Seiten. */
uint32_t pmm_free_page_count(void);

/** @return Gesamtzahl der verwalteten Seiten. */
uint32_t pmm_total_page_count(void);

/**
 * @brief Markiert einen physischen Seitenbereich als belegt (reserviert ihn).
 * @param start  Startadresse (abgerundet auf Seitengrenze).
 * @param end    Endadresse (aufgerundet auf Seitengrenze).
 */
void pmm_mark_used(uint32_t start, uint32_t end);

/**
 * @brief Markiert einen physischen Seitenbereich als frei.
 */
void pmm_mark_free(uint32_t start, uint32_t end);

#endif /* _MM_H */
