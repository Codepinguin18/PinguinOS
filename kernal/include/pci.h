/**
 * @file pci.h
 * @brief PCI-Bus-Enumerierung und Zugriff auf den Konfigurationsraum für PinguinOS.
 *
 * Verwendet den klassischen PCI Type 1 Konfigurationsmechanismus (I/O Ports 0xCF8 /
 * 0xCFC), der auf allen x86-Systemen ab den späten 1990ern verfügbar ist.
 *
 * PCI-Adressraum:
 *   Bits 31    = Enable-Bit (muss 1 sein)
 *   Bits 23:16 = Bus-Nummer (0–255)
 *   Bits 15:11 = Geräte-Nummer (0–31)
 *   Bits 10:8  = Funktions-Nummer (0–7)
 *   Bits 7:2   = Register-Offset (DWORD-ausgerichtet)
 */
#ifndef _PCI_H
#define _PCI_H

#include "types.h"

/* ── I/O Port Adressen ───────────────────────────────────────────── */
#define PCI_CFG_ADDR  0xCF8   /* Konfigurations-Adress-Port          */
#define PCI_CFG_DATA  0xCFC   /* Konfigurations-Daten-Port           */

/* ── Register-Offsets im Konfigurationsraum ──────────────────────── */
#define PCI_REG_VENDOR_ID    0x00
#define PCI_REG_DEVICE_ID    0x02
#define PCI_REG_COMMAND      0x04
#define PCI_REG_STATUS       0x06
#define PCI_REG_REVISION     0x08
#define PCI_REG_PROG_IF      0x09
#define PCI_REG_SUBCLASS     0x0A
#define PCI_REG_CLASS        0x0B
#define PCI_REG_CACHE_LINE   0x0C
#define PCI_REG_LATENCY      0x0D
#define PCI_REG_HEADER_TYPE  0x0E
#define PCI_REG_BIST         0x0F
#define PCI_REG_BAR0         0x10
#define PCI_REG_BAR1         0x14
#define PCI_REG_BAR2         0x18
#define PCI_REG_BAR3         0x1C
#define PCI_REG_BAR4         0x20
#define PCI_REG_BAR5         0x24
#define PCI_REG_SUBSYS_VID   0x2C
#define PCI_REG_SUBSYS_DID   0x2E
#define PCI_REG_INT_LINE     0x3C
#define PCI_REG_INT_PIN      0x3D

/* ── Bekannte Klassen-Codes ──────────────────────────────────────── */
#define PCI_CLASS_UNCLASSIFIED  0x00
#define PCI_CLASS_STORAGE       0x01
#define PCI_CLASS_NETWORK       0x02
#define PCI_CLASS_DISPLAY       0x03
#define PCI_CLASS_MULTIMEDIA    0x04
#define PCI_CLASS_BRIDGE        0x06
#define PCI_CLASS_SERIAL_BUS    0x0C

/* ── Verschiedenes ───────────────────────────────────────────────── */
#define PCI_VENDOR_NONE     0xFFFF   /* Kein Gerät vorhanden          */
#define PCI_MAX_BUS         256
#define PCI_MAX_DEVICE      32
#define PCI_MAX_FUNCTION    8

/* ── PCI-Geräte-Deskriptor ───────────────────────────────────────── */
typedef struct pci_device {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;

    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  revision;
    uint8_t  header_type;
    uint8_t  int_line;
    uint8_t  int_pin;

    uint32_t bar[6];         /* Basisadressregister (BARs)           */
} pci_device_t;

/** Maximale Anzahl an PCI-Geräten, die wir verfolgen. */
#define PCI_MAX_DEVICES  64

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** Enumeriert alle PCI-Busse und befüllt die Geräteliste. */
void pci_init(void);

/**
 * @brief Liest ein 32-Bit DWORD aus dem PCI-Konfigurationsraum.
 * @param bus, dev, func  PCI-Adresse.
 * @param reg  Register-Offset (muss DWORD-ausgerichtet sein).
 */
uint32_t pci_read32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Liest ein 16-Bit WORD aus dem PCI-Konfigurationsraum. */
uint16_t pci_read16(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Liest ein 8-Bit BYTE aus dem PCI-Konfigurationsraum. */
uint8_t  pci_read8 (uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Schreibt ein 32-Bit DWORD in den PCI-Konfigurationsraum. */
void pci_write32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg, uint32_t val);

/**
 * @brief Findet das erste Gerät, das zu Klasse/Subklasse passt.
 * @return Zeiger auf das passende pci_device_t, oder NULL wenn nicht gefunden.
 */
pci_device_t *pci_find_class(uint8_t class_code, uint8_t subclass);

/**
 * @brief Findet ein Gerät anhand von Vendor- + Device-ID.
 * @return Zeiger auf den passenden Eintrag, oder NULL.
 */
pci_device_t *pci_find_device(uint16_t vendor, uint16_t device_id);

/** @return Zeiger auf die interne Geräteliste (schreibgeschützt). */
const pci_device_t *pci_get_devices(void);

/** @return Anzahl der bei der Enumerierung gefundenen Geräte. */
uint32_t pci_device_count(void);

/** Gibt eine Zusammenfassung aller erkannten PCI-Geräte im Serial-Log aus. */
void pci_dump(void);

#endif /* _PCI_H */
