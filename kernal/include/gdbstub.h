/**
 * @file gdbstub.h
 * @brief GDB Remote-Debug-Stub für PinguinOS – Feature #93.
 *
 * Ermöglicht das Debuggen des Kernels von GDB aus über die
 * serielle Schnittstelle COM1.
 *
 * GDB-Verbindung:
 *   target remote /dev/ttyS0  (oder  target remote localhost:1234 via QEMU)
 *
 * Unterstützte GDB RSP-Pakete:
 *   g   – Register lesen
 *   G   – Register schreiben
 *   m   – Speicher lesen
 *   M   – Speicher schreiben
 *   c   – Ausführen fortsetzen
 *   s   – Einzelschritt
 *   ?   – Halt-Grund
 *   q   – Abfragen (Supported, Attached)
 */
#ifndef _GDBSTUB_H
#define _GDBSTUB_H

#include "types.h"

/** GDB-Stub initialisieren (registriert INT3/DEBUG-Handler). */
void gdbstub_init(void);

/** Aktiv in den GDB-Stub eintreten (Haltepunkt setzen). */
void gdbstub_breakpoint(void);

/** Gibt zurück ob GDB gerade verbunden ist. */
bool gdbstub_connected(void);

#endif /* _GDBSTUB_H */