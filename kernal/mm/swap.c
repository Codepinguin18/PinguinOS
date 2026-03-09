/**
 * @file swap.c
 * @brief Auslagerungspartition für PinguinOS – Feature #4.
 *
 * Auslagerungsslots:
 *   Jeder Slot = 4 KB = 8 IDE-Sektoren.
 *   Eine Bitmap verwaltet freie/belegte Slots.
 *   Der PTE einer ausgelagerten Seite wird auf 0 (nicht-vorhanden) gesetzt;
 *   der Slot-Index wird in einer separaten Tabelle gespeichert.
 *
 * Aufruf durch den Page-Fault-Handler:
 *   Wenn PTE == 0 aber swap_lookup(virt) >= 0 → swap_in() aufrufen.
 */

#include "../include/swap.h"
#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/ide.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/sync.h"

/* ── Konfiguration ────────────────────────────────────────────── */
#define SWAP_SLOTS_MAX     4096    /* 4096 × 4 KB = 16 MB Swap         */
#define SEKTOREN_PRO_SLOT  8       /* 8 × 512 Byte = 4096 Byte / Slot  */
#define SWAP_MAGIC_STR     "SWAP"  /* Partitions-Kennung               */

/* ── Swap-Lookup-Tabelle: virt → slot ────────────────────────── */
#define SWAP_LOOKUP_SIZE   1024    /* Bis zu 1024 ausgelagerte Seiten  */
typedef struct { uint32_t virt; int32_t slot; } swap_entry_t;
static swap_entry_t lookup[SWAP_LOOKUP_SIZE];

/* ── Modul-Zustand ────────────────────────────────────────────── */
static bool       swap_bereit     = false;
static uint8_t    swap_laufwerk   = 0;
static uint32_t   swap_lba_start  = 0;
static uint32_t   swap_slot_count = 0;
static uint8_t    slot_bitmap[SWAP_SLOTS_MAX / 8];
static spinlock_t swap_sperre     = SPINLOCK_INIT;
static uint32_t   ausgelagert     = 0;    /* Statistik: Seiten ausgelagert */
static uint32_t   eingelagert     = 0;    /* Statistik: Seiten eingelagert */

/* ── Hilfsfunktionen ─────────────────────────────────────────── */
static int slot_allozieren(void)
{
    for (uint32_t i = 0; i < swap_slot_count; i++) {
        uint32_t byte = i >> 3, bit = i & 7;
        if (!(slot_bitmap[byte] & (1u << bit))) {
            slot_bitmap[byte] |= (1u << bit);
            return (int)i;
        }
    }
    return -1;   /* Kein freier Slot */
}

static void slot_freigeben(uint32_t slot)
{
    if (slot < swap_slot_count)
        slot_bitmap[slot >> 3] &= ~(1u << (slot & 7));
}

/* Lookup-Eintrag für eine virtuelle Adresse setzen */
static void lookup_set(uint32_t virt, int32_t slot)
{
    for (int i = 0; i < SWAP_LOOKUP_SIZE; i++) {
        if (!lookup[i].virt || lookup[i].virt == virt) {
            lookup[i].virt = virt;
            lookup[i].slot = slot;
            return;
        }
    }
}

/* Slot für eine virtuelle Adresse nachschlagen (-1 = nicht vorhanden) */
static int32_t lookup_get(uint32_t virt)
{
    for (int i = 0; i < SWAP_LOOKUP_SIZE; i++)
        if (lookup[i].virt == virt) return lookup[i].slot;
    return -1;
}

/* ── Öffentlich: swap_init ───────────────────────────────────── */
void swap_init(uint8_t drive, uint32_t lba, uint32_t size_sectors)
{
    if (size_sectors < SEKTOREN_PRO_SLOT) return;

    /* Magic-Header auf die Partition schreiben */
    uint8_t header[512];
    memset(header, 0, sizeof(header));
    memcpy(header, SWAP_MAGIC_STR, 4);
    header[4] = (uint8_t)(size_sectors & 0xFF);
    ide_write(drive, lba, 1, header);

    swap_laufwerk   = drive;
    swap_lba_start  = lba + 1;   /* Erster Slot nach dem Header */
    swap_slot_count = (size_sectors - 1) / SEKTOREN_PRO_SLOT;
    if (swap_slot_count > SWAP_SLOTS_MAX) swap_slot_count = SWAP_SLOTS_MAX;

    memset(slot_bitmap, 0, sizeof(slot_bitmap));
    memset(lookup,      0, sizeof(lookup));
    swap_bereit = true;

    KINFO("Swap: Laufwerk=%u  LBA=%u  Slots=%u  (%u MB)\n",
          drive, swap_lba_start, swap_slot_count,
          (swap_slot_count * 4) / 1024);
}

/* ── Öffentlich: swap_out ────────────────────────────────────── */
int swap_out(uint32_t virt)
{
    if (!swap_bereit) return -1;

    /* Physischen Frame ermitteln */
    uint32_t phys = paging_get_phys(virt & PAGE_MASK);
    if (!phys) return -1;

    /* Freien Slot holen */
    uint32_t flags = spin_lock_irqsave(&swap_sperre);
    int slot = slot_allozieren();
    spin_unlock_irqrestore(&swap_sperre, flags);
    if (slot < 0) return -1;

    /* Seiteninhalt auf Disk schreiben */
    uint32_t lba = swap_lba_start + (uint32_t)slot * SEKTOREN_PRO_SLOT;
    if (ide_write(swap_laufwerk, lba, SEKTOREN_PRO_SLOT, (uint8_t *)phys) != 0) {
        flags = spin_lock_irqsave(&swap_sperre);
        slot_freigeben((uint32_t)slot);
        spin_unlock_irqrestore(&swap_sperre, flags);
        return -1;
    }

    /* Seite aus der virtuellen Adressraumstruktur entfernen */
    paging_unmap_page(virt & PAGE_MASK);
    pmm_free_page(phys);

    /* Slot in Lookup-Tabelle merken */
    flags = spin_lock_irqsave(&swap_sperre);
    lookup_set(virt & PAGE_MASK, slot);
    ausgelagert++;
    spin_unlock_irqrestore(&swap_sperre, flags);

    KINFO("Swap: ausgelagert virt=0x%08x → Slot %d\n", virt, slot);
    return 0;
}

/* ── Öffentlich: swap_in ─────────────────────────────────────── */
int swap_in(uint32_t virt, uint32_t swap_slot)
{
    if (!swap_bereit) return -1;

    /* Neuen physischen Frame allozieren */
    uint32_t phys = pmm_alloc_page();
    if (!phys) return -1;

    /* Seite von Disk laden */
    uint32_t lba = swap_lba_start + swap_slot * SEKTOREN_PRO_SLOT;
    if (ide_read(swap_laufwerk, lba, SEKTOREN_PRO_SLOT, (uint8_t *)phys) != 0) {
        pmm_free_page(phys);
        return -1;
    }

    /* Seite neu einmappen (Benutzer-Schreibrecht) */
    paging_map_page(virt & PAGE_MASK, phys, PTE_PRESENT | PTE_WRITABLE | PTE_USER);

    /* Slot freigeben */
    uint32_t flags = spin_lock_irqsave(&swap_sperre);
    slot_freigeben(swap_slot);
    lookup_set(virt & PAGE_MASK, -1);   /* Lookup-Eintrag löschen */
    eingelagert++;
    spin_unlock_irqrestore(&swap_sperre, flags);

    KINFO("Swap: eingelagert virt=0x%08x  Slot=%u\n", virt, swap_slot);
    return 0;
}

bool swap_ready_p(void) { return swap_bereit; }

void swap_dump(void)
{
    if (!swap_bereit) { serial_puts("[SWAP] nicht initialisiert\n"); return; }

    uint32_t belegt = 0;
    for (uint32_t i = 0; i < swap_slot_count; i++)
        if (slot_bitmap[i>>3] & (1u << (i&7))) belegt++;

    char buf[96];
    snprintf(buf, sizeof(buf),
             "[SWAP] Slots=%u  belegt=%u  aus=%u  ein=%u\n",
             swap_slot_count, belegt, ausgelagert, eingelagert);
    serial_puts(buf);
}