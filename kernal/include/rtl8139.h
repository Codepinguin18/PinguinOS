/**
 * @file rtl8139.h
 * @brief RTL8139-Netzwerkkarten-Treiber – Feature #45.
 */
#ifndef _RTL8139_H
#define _RTL8139_H

#include "types.h"

/** RTL8139 initialisieren (sucht PCI 0x10EC:0x8139). */
int  rtl8139_init(void);

/** Paket über RTL8139 senden. */
int  rtl8139_send(const uint8_t *data, uint16_t len);

/** MAC-Adresse auslesen (6 Byte in buf). */
void rtl8139_get_mac(uint8_t *buf);

#endif /* _RTL8139_H */