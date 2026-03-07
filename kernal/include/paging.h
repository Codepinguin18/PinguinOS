/**
 * @file paging.h
 * @brief x86 32-Bit Paging (zweistufige Seitentabellen) für PinguinOS.
 *
 * Wir verwenden einen einfach identitäts-gemappten Kernel-Space (virtuell == physisch für
 * die ersten 4 MB) plus ein Higher-Half-Mapping des gesamten Kernels.
 *
 * Layout:
 *   0x00000000 – 0x003FFFFF  Identitäts-gemappt (Kernel-Bootstrap)
 *   0xC0000000 – 0xFFFFFFFF  Higher-Half-Kernel-Fenster
 *
 * Jeder Prozess wird schließlich sein eigenes Seitenverzeichnis erhalten; die
 * Kernel-Seitenverzeichniseinträge werden von allen Prozessen geteilt.
 */
#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/* ── Seitenverzeichnis / Seitentabelleneintrag-Flags ─────────────── */
#define PDE_PRESENT    (1 << 0)   /* Eintrag ist gültig               */
#define PDE_WRITABLE   (1 << 1)   /* Bereich ist beschreibbar         */
#define PDE_USER       (1 << 2)   /* Aus dem User-Modus zugreifbar    */
#define PDE_WRITETHRU  (1 << 3)   /* Write-through Caching            */
#define PDE_NOCACHE    (1 << 4)   /* Cache deaktiviert                */
#define PDE_ACCESSED   (1 << 5)   /* Durch CPU bei Zugriff gesetzt    */
#define PDE_DIRTY      (1 << 6)   /* Durch CPU bei Schreibzugriff gesetzt (nur PTEs) */
#define PDE_HUGE       (1 << 7)   /* 4 MB Seite (nur PDEs)            */
#define PDE_GLOBAL     (1 << 8)   /* Nicht aus TLB löschen bei cr3-Ladung */

/* Anzahl der Einträge in einem Seitenverzeichnis / einer Seitentabelle */
#define PD_ENTRIES   1024
#define PT_ENTRIES   1024

/* Extrahiert den Seitenverzeichnis-Index aus einer virtuellen Adresse */
#define PD_INDEX(va)   (((uint32_t)(va)) >> 22)
/* Extrahiert den Seitentabellen-Index aus einer virtuellen Adresse */
#define PT_INDEX(va)   ((((uint32_t)(va)) >> 12) & 0x3FF)
/* Extrahiert den Byte-Offset innerhalb einer Seite */
#define PAGE_OFFSET(va) (((uint32_t)(va)) & 0xFFF)

/* ── Typen ───────────────────────────────────────────────────────── */
typedef uint32_t pde_t;   /* Seitenverzeichniseintrag */
typedef uint32_t pte_t;   /* Seitentabelleneintrag    */

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief Initialisiert das Paging und aktiviert das CR0.PG-Bit.
 *
 * Richtet das Kernel-Seitenverzeichnis ein mit:
 *  – einer Identitäts-Map der ersten 4 MB (damit der Code, der direkt nach
 *    dem Aktivieren des Pagings ausgeführt wird, weiterhin funktioniert),
 *  – einem 4 MB Mapping an der Higher-Half-Kernel-Basis.
 *
 * Nach diesem Aufruf mappen virtuelle Adressen 0xC0000000 + Offset auf den
 * gleichen physischen Speicher wie 0x00000000 + Offset.
 */
void paging_init(void);

/**
 * @brief Mappt eine einzelne virtuelle Seite auf eine physische Seite im Kernel-PD.
 * @param virt   Virtuelle Adresse (seiten-ausgerichtet).
 * @param phys   Physische Adresse (seiten-ausgerichtet).
 * @param flags  Kombination von PDE_* Flags.
 */
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags);

/**
 * @brief Hebt das Mapping einer einzelnen virtuellen Seite auf.
 * @param virt  Virtuelle Adresse (seiten-ausgerichtet).
 */
void paging_unmap(uint32_t virt);

/**
 * @brief Leert den gesamten TLB durch erneutes Laden von CR3.
 */
void paging_flush_tlb(void);

/**
 * @brief Invalidiert einen einzelnen TLB-Eintrag für die gegebene virtuelle Adresse.
 */
void paging_invalidate(uint32_t virt);

/**
 * @brief Gibt die physische Adresse zurück, die im Kernel-PD auf @p virt gemappt ist,
 *        oder 0, wenn nicht gemappt.
 */
uint32_t paging_get_phys(uint32_t virt);

/** @return Physische Adresse des Kernel-Seitenverzeichnisses. */
uint32_t paging_get_cr3(void);

#endif /* _PAGING_H */
