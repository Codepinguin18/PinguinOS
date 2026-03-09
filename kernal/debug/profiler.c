/**
 * @file profiler.c
 * @brief Kernel-Sampling-Profiler für PinguinOS – Feature #96.
 *
 * Funktionsprinzip (Sampling-Profiler):
 *   Beim jedem Timer-IRQ (1000 Hz) wird der aktuelle Instruktions-
 *   zeiger (EIP) des unterbrochenen Tasks aufgezeichnet.
 *   EIP >> PROFILER_SHIFT wird als Index in ein Bucket-Array verwendet.
 *   Nach der Messung zeigt profiler_dump() die "heißen" Stellen.
 *
 *   Einbindung in scheduler.c timer_handler():
 *     extern void profiler_sample(uint32_t eip);
 *     profiler_sample(regs->eip);
 */

#include "../include/profiler.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/sync.h"

/* ── Bucket-Array ────────────────────────────────────────────── */
typedef struct {
    uint32_t eip_basis;   /* Basis-EIP dieses Buckets */
    uint32_t treffer;     /* Anzahl Samples           */
} profiler_bucket_t;

static profiler_bucket_t buckets[PROFILER_BUCKETS];
static uint32_t          gesamt_samples = 0;
static bool              aktiv          = false;
static spinlock_t        prof_sperre    = SPINLOCK_INIT;

/* ── Öffentlich: profiler_init ───────────────────────────────── */
void profiler_init(void)
{
    memset(buckets, 0, sizeof(buckets));
    gesamt_samples = 0;
    aktiv          = false;
    KINFO("Profiler: initialisiert (%u Buckets, Granularität=%u Byte)\n",
          PROFILER_BUCKETS, 1u << PROFILER_SHIFT);
}

void profiler_start(void) { aktiv = true;  }
void profiler_stop(void)  { aktiv = false; }

/* ── Öffentlich: profiler_sample ─────────────────────────────── */
void profiler_sample(uint32_t eip)
{
    if (!aktiv) return;

    /* Bucket-Index: EIP in Blöcke aufteilen */
    uint32_t idx = (eip >> PROFILER_SHIFT) & (PROFILER_BUCKETS - 1);

    uint32_t flags = spin_lock_irqsave(&prof_sperre);
    if (buckets[idx].treffer == 0)
        buckets[idx].eip_basis = eip & ~((1u << PROFILER_SHIFT) - 1);
    buckets[idx].treffer++;
    gesamt_samples++;
    spin_unlock_irqrestore(&prof_sperre, flags);
}

/* ── Öffentlich: profiler_dump ───────────────────────────────── */
void profiler_dump(uint32_t top_n)
{
    if (top_n == 0 || top_n > PROFILER_BUCKETS) top_n = 10;

    char buf[80];
    snprintf(buf, sizeof(buf),
             "[PROFILER] %u Samples total\n", gesamt_samples);
    serial_puts(buf);

    if (gesamt_samples == 0) return;

    /* Einfaches Selection-Sort für Top-N */
    bool gezeigt[PROFILER_BUCKETS] = {false};

    for (uint32_t n = 0; n < top_n; n++) {
        uint32_t best_idx     = 0;
        uint32_t best_treffer = 0;

        for (uint32_t i = 0; i < PROFILER_BUCKETS; i++) {
            if (!gezeigt[i] && buckets[i].treffer > best_treffer) {
                best_treffer = buckets[i].treffer;
                best_idx     = i;
            }
        }

        if (best_treffer == 0) break;
        gezeigt[best_idx] = true;

        uint32_t prozent = (best_treffer * 100) / gesamt_samples;
        snprintf(buf, sizeof(buf),
                 "  #%u  EIP=0x%08x  Treffer=%u  (%u%%)\n",
                 n + 1, buckets[best_idx].eip_basis,
                 best_treffer, prozent);
        serial_puts(buf);
    }
}

void profiler_reset(void)
{
    uint32_t flags = spin_lock_irqsave(&prof_sperre);
    memset(buckets, 0, sizeof(buckets));
    gesamt_samples = 0;
    spin_unlock_irqrestore(&prof_sperre, flags);
}