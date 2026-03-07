/**
 * @file e1000.h
 * @brief Intel E1000 (82540EM) NIC driver API for PinguinOS.
 *
 * Feature #44 – E1000 NIC
 *   PCI vendor:device = 8086:100E (the default QEMU NIC).
 *   Provides a net_iface_t that is registered with the network stack.
 */
#ifndef _E1000_H
#define _E1000_H

#include "types.h"
#include "pci.h"
#include "net.h"

/**
 * @brief Probe for an E1000 PCI device and initialise the driver.
 *
 * If an E1000 is found the driver allocates descriptor rings, reads the
 * MAC address from EEPROM, and registers a net_iface_t with net_init().
 *
 * @return true if an E1000 was found and initialised, false otherwise.
 */
bool e1000_init(void);

/**
 * @brief Transmit a raw Ethernet frame.
 * @param data  Pointer to frame data (must include Ethernet header).
 * @param len   Length in bytes (max 1514 for standard frames).
 * @return true on success.
 */
bool e1000_send(const void *data, uint32_t len);

/** @return Pointer to the 6-byte MAC address of this interface. */
const uint8_t *e1000_get_mac(void);

#endif /* _E1000_H */