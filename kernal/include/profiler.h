/**
 * @file profiler.h
 * @brief Sampling-Profiler für den PinguinOS-Kernel – Feature #96.
 *
 * Der Profiler wird vom Timer-IRQ aufgerufen und zeichnet den
 * aktuellen EIP auf.  Über profiler_dump() kann eine Statistik
 * der häufigsten Ausführungsstellen ausgegeben werden.
 */
#ifndef _PROFILER_H
#define _PROFILER_H

#include "types.h"

#define PROFILER_BUCKETS  256   /* Anzahl EIP-Buckets       */
#define PROFILER_SHIFT    4     /* Bucketing-Granularität   */

/** Profiler initialisieren. */
void profiler_init(void);

/** Profiler aktivieren. */
void profiler_start(void);

/** Profiler anhalten. */
void profiler_stop(void);

/** EIP-Sample hinzufügen (wird vom Timer-Handler aufgerufen). */
void profiler_sample(uint32_t eip);

/** Top-N-Buckets auf seriell ausgeben. */
void profiler_dump(uint32_t top_n);

/** Alle Zähler zurücksetzen. */
void profiler_reset(void);

#endif /* _PROFILER_H */