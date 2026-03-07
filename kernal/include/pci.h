/**
 * @file pci.h
 * @brief PCI bus enumeration and configuration-space access for PinguinOS.
 *
 * Uses the legacy PCI Type 1 configuration mechanism (I/O ports 0xCF8 /
 * 0xCFC) which is available on all x86 systems from the late 1990s onwards.
 *
 * PCI address space:
 *   Bits 31    = Enable bit (must be 1)
 *   Bits 23:16 = Bus  number (0–255)
 *   Bits 15:11 = Device number (0–31)
 *   Bits 10:8  = Function number (0–7)
 *   Bits 7:2   = Register offset (DWORD-aligned)
 */
#ifndef _PCI_H
#define _PCI_H

#include "types.h"

/* ── I/O port addresses ──────────────────────────────────────────── */
#define PCI_CFG_ADDR  0xCF8   /* Configuration address port          */
#define PCI_CFG_DATA  0xCFC   /* Configuration data port             */

/* ── Configuration space register offsets ───────────────────────── */
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

/* ── Well-known class codes ──────────────────────────────────────── */
#define PCI_CLASS_UNCLASSIFIED  0x00
#define PCI_CLASS_STORAGE       0x01
#define PCI_CLASS_NETWORK       0x02
#define PCI_CLASS_DISPLAY       0x03
#define PCI_CLASS_MULTIMEDIA    0x04
#define PCI_CLASS_BRIDGE        0x06
#define PCI_CLASS_SERIAL_BUS    0x0C

/* ── Misc ────────────────────────────────────────────────────────── */
#define PCI_VENDOR_NONE     0xFFFF   /* No device present             */
#define PCI_MAX_BUS         256
#define PCI_MAX_DEVICE      32
#define PCI_MAX_FUNCTION    8

/* ── PCI device descriptor ───────────────────────────────────────── */
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

    uint32_t bar[6];         /* Base Address Registers               */
} pci_device_t;

/** Maximum number of PCI devices we track. */
#define PCI_MAX_DEVICES  64

/* ── Public API ──────────────────────────────────────────────────── */

/** Enumerate all PCI buses and populate the device list. */
void pci_init(void);

/**
 * @brief Read a 32-bit DWORD from PCI configuration space.
 * @param bus, dev, func  PCI address.
 * @param reg  Register offset (must be DWORD-aligned).
 */
uint32_t pci_read32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Read a 16-bit WORD from PCI config space. */
uint16_t pci_read16(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Read an 8-bit BYTE from PCI config space. */
uint8_t  pci_read8 (uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);

/** Write a 32-bit DWORD to PCI config space. */
void pci_write32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg, uint32_t val);

/**
 * @brief Find the first device matching class/subclass.
 * @return Pointer to the matching pci_device_t, or NULL if not found.
 */
pci_device_t *pci_find_class(uint8_t class_code, uint8_t subclass);

/**
 * @brief Find a device by vendor + device ID.
 * @return Pointer to the matching entry, or NULL.
 */
pci_device_t *pci_find_device(uint16_t vendor, uint16_t device_id);

/** @return Pointer to the internal device list (read-only). */
const pci_device_t *pci_get_devices(void);

/** @return Number of devices found during enumeration. */
uint32_t pci_device_count(void);

/** Print a summary of all detected PCI devices to the serial log. */
void pci_dump(void);

#endif /* _PCI_H */
