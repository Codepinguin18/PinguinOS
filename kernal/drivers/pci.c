/**
 * @file pci.c
 * @brief PCI bus enumeration for PinguinOS.
 */

#include "../include/pci.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"

/* ── Device table ────────────────────────────────────────────────── */
static pci_device_t devices[PCI_MAX_DEVICES];
static uint32_t     device_count = 0;

/* ── Configuration space access ──────────────────────────────────── */
uint32_t pci_read32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg)
{
    uint32_t addr = (1U << 31)
                  | ((uint32_t)bus  << 16)
                  | ((uint32_t)dev  << 11)
                  | ((uint32_t)func <<  8)
                  | ((uint32_t)reg  &  0xFC);
    outl(PCI_CFG_ADDR, addr);
    return inl(PCI_CFG_DATA);
}

uint16_t pci_read16(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg)
{
    uint32_t val = pci_read32(bus, dev, func, reg & ~0x3U);
    return (uint16_t)((val >> ((reg & 2) * 8)) & 0xFFFF);
}

uint8_t pci_read8(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg)
{
    uint32_t val = pci_read32(bus, dev, func, reg & ~0x3U);
    return (uint8_t)((val >> ((reg & 3) * 8)) & 0xFF);
}

void pci_write32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg, uint32_t val)
{
    uint32_t addr = (1U << 31)
                  | ((uint32_t)bus  << 16)
                  | ((uint32_t)dev  << 11)
                  | ((uint32_t)func <<  8)
                  | ((uint32_t)reg  &  0xFC);
    outl(PCI_CFG_ADDR, addr);
    outl(PCI_CFG_DATA, val);
}

/* ── Enumerate one function ──────────────────────────────────────── */
static void pci_check_function(uint8_t bus, uint8_t dev, uint8_t func)
{
    uint16_t vendor = pci_read16(bus, dev, func, PCI_REG_VENDOR_ID);
    if (vendor == PCI_VENDOR_NONE) return;
    if (device_count >= PCI_MAX_DEVICES) return;

    pci_device_t *d = &devices[device_count++];
    d->bus        = bus;
    d->device     = dev;
    d->function   = func;
    d->vendor_id  = vendor;
    d->device_id  = pci_read16(bus, dev, func, PCI_REG_DEVICE_ID);
    d->class_code = pci_read8 (bus, dev, func, PCI_REG_CLASS);
    d->subclass   = pci_read8 (bus, dev, func, PCI_REG_SUBCLASS);
    d->prog_if    = pci_read8 (bus, dev, func, PCI_REG_PROG_IF);
    d->revision   = pci_read8 (bus, dev, func, PCI_REG_REVISION);
    d->header_type= pci_read8 (bus, dev, func, PCI_REG_HEADER_TYPE);
    d->int_line   = pci_read8 (bus, dev, func, PCI_REG_INT_LINE);
    d->int_pin    = pci_read8 (bus, dev, func, PCI_REG_INT_PIN);

    for (int i = 0; i < 6; i++)
        d->bar[i] = pci_read32(bus, dev, func, PCI_REG_BAR0 + i * 4);
}

/* ── Public: pci_init ────────────────────────────────────────────── */
void pci_init(void)
{
    device_count = 0;
    memset(devices, 0, sizeof(devices));

    for (uint32_t bus = 0; bus < PCI_MAX_BUS; bus++) {
        for (uint32_t dev = 0; dev < PCI_MAX_DEVICE; dev++) {
            uint16_t vendor = pci_read16((uint8_t)bus, (uint8_t)dev, 0, PCI_REG_VENDOR_ID);
            if (vendor == PCI_VENDOR_NONE) continue;

            /* Check if multi-function */
            uint8_t htype = pci_read8((uint8_t)bus, (uint8_t)dev, 0, PCI_REG_HEADER_TYPE);
            uint8_t max_func = (htype & 0x80) ? PCI_MAX_FUNCTION : 1;

            for (uint8_t func = 0; func < max_func; func++)
                pci_check_function((uint8_t)bus, (uint8_t)dev, func);
        }
    }

    KINFO("PCI: found %u device(s)\n", device_count);
}

/* ── Lookup helpers ──────────────────────────────────────────────── */
pci_device_t *pci_find_class(uint8_t class_code, uint8_t subclass)
{
    for (uint32_t i = 0; i < device_count; i++)
        if (devices[i].class_code == class_code && devices[i].subclass == subclass)
            return &devices[i];
    return NULL;
}

pci_device_t *pci_find_device(uint16_t vendor, uint16_t device_id)
{
    for (uint32_t i = 0; i < device_count; i++)
        if (devices[i].vendor_id == vendor && devices[i].device_id == device_id)
            return &devices[i];
    return NULL;
}

const pci_device_t *pci_get_devices(void)  { return devices;      }
uint32_t            pci_device_count(void) { return device_count; }

void pci_dump(void)
{
    KINFO("PCI device list (%u entries):\n", device_count);
    for (uint32_t i = 0; i < device_count; i++) {
        pci_device_t *d = &devices[i];
        KINFO("  [%02x:%02x.%x] %04x:%04x  class %02x.%02x  IRQ %u\n",
              d->bus, d->device, d->function,
              d->vendor_id, d->device_id,
              d->class_code, d->subclass,
              d->int_line);
    }
}
