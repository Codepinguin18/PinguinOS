/**
 * @file apic.h
 * @brief Local APIC und APIC-Timer – Feature #86.
 *
 * Der Local APIC ist in moderne x86-CPUs integriert und ermöglicht
 * genauere Timer-Interrupts als der PIT.  apic_init() aktiviert den
 * APIC und kalibriert den internen Timer mithilfe des PIT.
 */
#ifndef _APIC_H
#define _APIC_H

#include "types.h"

/** Prüfen ob ein Local APIC vorhanden ist (CPUID). */
bool apic_detect(void);

/** Local APIC initialisieren und APIC-Timer kalibrieren. */
void apic_init(void);

/** EOI (End of Interrupt) an den Local APIC senden. */
void apic_eoi(void);

/** APIC-Timer-Frequenz in Hz zurückgeben. */
uint32_t apic_timer_hz(void);

/** APIC-ID der aktuellen CPU zurückgeben. */
uint8_t apic_id(void);

/** APIC-Zustand auf seriell ausgeben. */
void apic_dump(void);

#endif /* _APIC_H */