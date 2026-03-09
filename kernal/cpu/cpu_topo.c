/**
 * @file cpu_topo.c
 * @brief CPU-Topologie-Erkennung für PinguinOS – Feature #89.
 *
 * Ermittelt via CPUID-Blatt 0x1 und 0xB (Extended Topology Enumeration):
 *   - Anzahl logischer CPUs (CPUID.0x1.EBX[23:16])
 *   - Hyper-Threading (CPUID.0x1.EDX[28])
 *   - Physische Kerne (aus HTT-Flag und logischer CPU-Zahl)
 *   - Modellname aus CPUID-Blättern 0x80000002–0x80000004
 */

#include "../include/cpu_topo.h"
#include "../include/cpu.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Topologie-Daten ─────────────────────────────────────────── */
static cpu_topo_t topo;

/* ── CPUID-Hilfsfunktion ─────────────────────────────────────── */
static INLINE void cpuid4(uint32_t leaf, uint32_t *a, uint32_t *b,
                            uint32_t *c, uint32_t *d)
{
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(leaf), "c"(0)
    );
}

/* ── Öffentlich: cpu_topo_init ───────────────────────────────── */
void cpu_topo_init(void)
{
    memset(&topo, 0, sizeof(topo));

    uint32_t eax, ebx, ecx, edx;

    /* ── Logische CPUs aus CPUID 0x1 ────────────────────────── */
    cpuid4(0x1, &eax, &ebx, &ecx, &edx);
    uint32_t logical_per_pkg = (ebx >> 16) & 0xFF;
    bool htt = !!(edx & (1u << 28));   /* HTT-Flag in EDX bit 28 */

    topo.logical_cpus     = (logical_per_pkg > 0) ? logical_per_pkg : 1;
    topo.hyperthreading   = htt;
    topo.packages         = 1;   /* Vereinfachung: 1 Paket (kein SMP) */

    if (htt && topo.logical_cpus > 1) {
        /* Bei HTT: halb so viele physische Kerne wie logische CPUs */
        topo.physical_cores   = topo.logical_cpus / 2;
        topo.threads_per_core = 2;
    } else {
        topo.physical_cores   = topo.logical_cpus;
        topo.threads_per_core = 1;
    }

    /* ── CPU-Modellname aus erweiterten CPUID-Blättern ─────── */
    /* CPUID 0x80000000: prüfen ob erweiterte Blätter verfügbar */
    cpuid4(0x80000000u, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000004u) {
        /* Modellname: 48 ASCII-Bytes aus Blättern 0x80000002–0x80000004 */
        uint32_t *dst = (uint32_t *)topo.model_name;
        cpuid4(0x80000002u, &dst[0], &dst[1], &dst[2], &dst[3]);
        cpuid4(0x80000003u, &dst[4], &dst[5], &dst[6], &dst[7]);
        cpuid4(0x80000004u, &dst[8], &dst[9], &dst[10], &dst[11]);
        topo.model_name[47] = '\0';
        /* Führende Leerzeichen entfernen */
        char *p = topo.model_name;
        while (*p == ' ') p++;
        if (p != topo.model_name) memmove(topo.model_name, p, strlen(p)+1);
    } else {
        strncpy(topo.model_name, "Unbekannte CPU", 63);
    }

    KINFO("CPU-Topo: Kerne=%u  Threads/Kern=%u  logisch=%u  HT=%s\n",
          topo.physical_cores, topo.threads_per_core,
          topo.logical_cpus, topo.hyperthreading ? "ja" : "nein");
}

const cpu_topo_t *cpu_topo_get(void) { return &topo; }

void cpu_topo_dump(void)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "[CPU-TOPO] '%s'\n"
             "  Pakete=%u  physisch=%u  logisch=%u  HT=%s\n",
             topo.model_name,
             topo.packages, topo.physical_cores,
             topo.logical_cpus,
             topo.hyperthreading ? "ja" : "nein");
    serial_puts(buf);
}