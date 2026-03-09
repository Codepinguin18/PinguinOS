/**
 * @file ahci.h
 * @brief AHCI/SATA-Controller-Treiber – Feature #42.
 *
 * Unterstützt den AHCI-Controller (PCI-Klasse 0x01, Unterklasse 0x06).
 * Erkennt bis zu AHCI_MAX_PORTS Laufwerke und stellt ide-kompatible
 * Lese-/Schreibfunktionen bereit.
 */
#ifndef _AHCI_H
#define _AHCI_H

#include "types.h"

/* Maximale Anzahl unterstützter AHCI-Ports */
#define AHCI_MAX_PORTS   32

/**
 * @brief AHCI-Controller initialisieren.
 * @return Anzahl erkannter Laufwerke, 0 wenn kein AHCI vorhanden.
 */
int  ahci_init(void);

/**
 * @brief Sektoren von einem AHCI-Laufwerk lesen.
 * @param port   Port-Nummer (0–31)
 * @param lba    Startsektor
 * @param count  Anzahl Sektoren (je 512 Byte)
 * @param buf    Zielpuffer (mindestens count×512 Byte)
 * @return 0 bei Erfolg, -1 bei Fehler
 */
int  ahci_read (uint8_t port, uint64_t lba, uint32_t count, uint8_t *buf);

/**
 * @brief Sektoren auf ein AHCI-Laufwerk schreiben.
 * @param port   Port-Nummer (0–31)
 * @param lba    Startsektor
 * @param count  Anzahl Sektoren
 * @param buf    Quellpuffer
 * @return 0 bei Erfolg, -1 bei Fehler
 */
int  ahci_write(uint8_t port, uint64_t lba, uint32_t count, const uint8_t *buf);

/** Anzahl erkannter AHCI-Laufwerke zurückgeben. */
uint32_t ahci_drive_count(void);

/** AHCI-Status auf serieller Schnittstelle ausgeben. */
void ahci_dump(void);

#endif /* _AHCI_H */