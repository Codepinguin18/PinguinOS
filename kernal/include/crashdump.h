/**
 * @file crashdump.h
 * @brief Automatische Absturzberichte für PinguinOS – Feature #98.
 *
 * Bei jedem kpanic() wird ein vollständiger Crash-Dump erzeugt:
 *   - Datum/Uhrzeit (RTC)
 *   - Fehlermeldung
 *   - Vollständiger Register-Dump
 *   - Stack-Backtrace (bis 20 Frames)
 *   - Speicherauslastung
 *   - Laufende Tasks
 *
 * Der Dump wird auf die serielle Schnittstelle und (falls verfügbar)
 * in eine Datei /crash.log auf der RAM-Disk geschrieben.
 */
#ifndef _CRASHDUMP_H
#define _CRASHDUMP_H

#include "types.h"
#include "idt.h"

/** Crash-Dump-Subsystem initialisieren (verbindet sich mit kpanic). */
void crashdump_init(void);

/** Crash-Dump manuell auslösen (z.B. aus kpanic heraus). */
void crashdump_write(const char *nachricht, cpu_regs_t *regs);

#endif /* _CRASHDUMP_H */