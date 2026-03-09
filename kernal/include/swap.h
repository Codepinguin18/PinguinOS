/**
 * @file swap.h
 * @brief Auslagerungspartition (Swap) für PinguinOS – Feature #4.
 *
 * Seiten werden auf eine dedizierte IDE-Partition ausgelagert wenn
 * der physische Arbeitsspeicher knapp wird.
 *
 * Aufruf:
 *   swap_init(laufwerk, lba_start, größe_sektoren);
 *   swap_out(virtuelle_adresse);   – Seite auslagern
 *   swap_in(virtuelle_adresse, slot);   – Seite einlagern
 */
#ifndef _SWAP_H
#define _SWAP_H

#include "types.h"

/** Swap-Partition initialisieren. */
void swap_init(uint8_t drive, uint32_t lba, uint32_t size_sectors);

/** Seite auslagern (gibt physischen Frame frei). */
int  swap_out(uint32_t virt);

/** Seite einlagern (alloziert neuen physischen Frame). */
int  swap_in(uint32_t virt, uint32_t swap_slot);

/** Gibt zurück ob die Swap-Partition bereit ist. */
bool swap_ready_p(void);

/** Swap-Statistiken auf seriell ausgeben. */
void swap_dump(void);

#endif /* _SWAP_H */