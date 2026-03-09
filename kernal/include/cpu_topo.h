/**
 * @file cpu_topo.h
 * @brief CPU-Topologie-Erkennung (Kerne, Threads, Pakete) – Feature #89.
 */
#ifndef _CPU_TOPO_H
#define _CPU_TOPO_H

#include "types.h"

typedef struct {
    uint32_t  logical_cpus;     /* Logische CPUs (inkl. Hyper-Threading) */
    uint32_t  physical_cores;   /* Physische Kerne                       */
    uint32_t  threads_per_core; /* Threads pro Kern (HT)                 */
    uint32_t  packages;         /* Physische CPU-Pakete                  */
    bool      hyperthreading;   /* Hyper-Threading aktiv?                */
    char      model_name[64];   /* CPU-Modellbezeichnung                 */
} cpu_topo_t;

/** CPU-Topologie per CPUID ermitteln. */
void cpu_topo_init(void);

/** Zeiger auf ermittelte Topologie-Daten zurückgeben. */
const cpu_topo_t *cpu_topo_get(void);

/** Topologie auf seriell ausgeben. */
void cpu_topo_dump(void);

#endif /* _CPU_TOPO_H */