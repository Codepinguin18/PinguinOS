/**
 * @file apic.c
 * @brief Local APIC – Erkennung, Initialisierung und Timer – Feature #86.
 *
 * Der Local APIC befindet sich standardmäßig bei physischer Adresse
 * 0xFEE00000 (kann über MSR IA32_APIC_BASE verlegt werden).
 *
 * Kalibrierung des APIC-Timers:
 *   1. APIC-Timer starten (maximaler Zählwert, Teiler 16)
 *   2. 10 ms mit dem PIT warten
 *   3. APIC-Ticker-Wert lesen → ergibt Ticks pro 10 ms
 *   4. Hochrechnen auf 1 kHz (1 ms Intervall)
 *
 * Bugfixes:
 *   1. cpu_cpuid_edx() → cpuid() aus cpu.h (korrekter Name)
 *   2. rdmsr() → Inline-ASM (nicht separat deklariert)
 *   3. paging_map() + PTE_PRESENT/PTE_WRITABLE →
 *      paging_map_page() + PDE_PRESENT/PDE_WRITABLE/PDE_NOCACHE
 */

#include "../include/apic.h"
#include "../include/cpu.h"
#include "../include/idt.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/paging.h"   /* Fix #3: paging_map_page + PDE_*-Flags */

/* ── APIC-MMIO-Basis ─────────────────────────────────────────── */
#define APIC_BASE_PHYS   0xFEE00000u
#define APIC_BASE_MSR    0x1B          /* IA32_APIC_BASE MSR */

/* ── APIC-Register-Offsets ───────────────────────────────────── */
#define APIC_ID          0x020   /* APIC-ID                     */
#define APIC_VER         0x030   /* APIC-Version                */
#define APIC_SVR         0x0F0   /* Spurious Interrupt Vector   */
#define APIC_EOI_REG     0x0B0   /* End of Interrupt            */
#define APIC_LVT_TIMER   0x320   /* LVT Timer-Eintrag           */
#define APIC_TIMER_ICR   0x380   /* Timer Initial Count         */
#define APIC_TIMER_CCR   0x390   /* Timer Current Count         */
#define APIC_TIMER_DCR   0x3E0   /* Timer Divide Config         */

/* ── LVT-Timer-Bits ──────────────────────────────────────────── */
#define LVT_TIMER_VECTOR   0x20       /* IRQ0-Vektor (wie PIT)    */
#define LVT_TIMER_PERIODIC (1u << 17) /* Periodischer Modus       */
#define LVT_TIMER_MASKED   (1u << 16) /* Interrupt maskiert       */

/* ── SVR-Bits ────────────────────────────────────────────────── */
#define SVR_ENABLE   (1u << 8)   /* APIC aktivieren    */
#define SVR_SPURIOUS 0xFF        /* Spurious-Vektor    */

/* ── PIT-Kalibrierungskonstanten ─────────────────────────────── */
#define PIT_FREQ      1193182u
#define CALIB_MS      10         /* Kalibrierungsdauer in ms */
#define PIT_CMD       0x43
#define PIT_CH2       0x42
#define PIT_CTRL      0x61       /* PC-Speaker-Control       */

/* ── Interner Zustand ────────────────────────────────────────── */
static volatile uint32_t *apic     = NULL;
static uint32_t           timer_hz = 0;
static bool               apic_ok  = false;

/* ── APIC-Register lesen/schreiben ───────────────────────────── */
static INLINE uint32_t apic_read(uint32_t reg)
{ return apic[reg >> 2]; }

static INLINE void apic_write(uint32_t reg, uint32_t val)
{ apic[reg >> 2] = val; }

/* ── Fix #2: rdmsr() als lokale Hilfsfunktion ────────────────── */
/* Liest ein 64-Bit Model Specific Register (MSR) per RDMSR-Befehl. */
static INLINE uint64_t rdmsr(uint32_t msr_id)
{
    uint32_t lo, hi;
    __asm__ volatile (
        "rdmsr"
        : "=a"(lo), "=d"(hi)
        : "c"(msr_id)
    );
    return ((uint64_t)hi << 32) | (uint64_t)lo;
}

/* ── PIT-Kanal 2 für Kalibrierung (ohne IRQ) ─────────────────── */
static void pit_start_calib(uint32_t ms)
{
    uint32_t ticks = (PIT_FREQ * ms) / 1000u;
    /* Kanal 2, Modus 0 (One-Shot), Lobyte/Hibyte */
    outb(PIT_CMD, 0xB2);
    outb(PIT_CH2, (uint8_t)(ticks & 0xFF));
    outb(PIT_CH2, (uint8_t)(ticks >> 8));
    /* Gate für Kanal 2 einschalten */
    outb(PIT_CTRL, (uint8_t)((inb(PIT_CTRL) & ~0x02u) | 0x01u));
}

static bool pit_calib_done(void)
{
    /* Bit 5 des Ports 0x61 = OUT2 – wird HIGH wenn Timer abgelaufen */
    return !!(inb(PIT_CTRL) & 0x20u);
}

/* ── Öffentlich: apic_detect ─────────────────────────────────── */
bool apic_detect(void)
{
    /* Fix #1: cpuid() aus cpu.h nutzen statt cpu_cpuid_edx() */
    uint32_t eax, ebx, ecx, edx;
    cpuid(0x1, &eax, &ebx, &ecx, &edx);
    /* CPUID Blatt 1, EDX Bit 9 = Local APIC vorhanden */
    return !!(edx & (1u << 9));
}

/* ── Öffentlich: apic_init ───────────────────────────────────── */
void apic_init(void)
{
    if (!apic_detect()) {
        KERR("APIC: kein Local APIC in dieser CPU\n");
        return;
    }

    /* APIC-Basisadresse aus MSR lesen (Bits 35:12 = physische Seite) */
    uint64_t msr  = rdmsr(APIC_BASE_MSR);
    uint32_t base = (uint32_t)(msr & 0xFFFFF000u);

    /* Fix #3: paging_map_page() + PDE_*-Flags (nicht PTE_* / paging_map) */
    paging_map_page(base, base,
                    PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);

    apic = (volatile uint32_t *)base;

    /* APIC aktivieren: SVR-Register setzen */
    apic_write(APIC_SVR, SVR_ENABLE | SVR_SPURIOUS);

    /* ── APIC-Timer kalibrieren ──────────────────────────────── */
    /* Teiler auf 16 einstellen */
    apic_write(APIC_TIMER_DCR, 0x03);
    /* Timer maskieren (kein Interrupt während Kalibrierung) */
    apic_write(APIC_LVT_TIMER, LVT_TIMER_MASKED);
    /* Maximalen Zählwert laden */
    apic_write(APIC_TIMER_ICR, 0xFFFFFFFFu);

    /* CALIB_MS Millisekunden mit PIT-Kanal 2 abwarten */
    pit_start_calib(CALIB_MS);
    while (!pit_calib_done());

    /* Verbleibenden Zählerwert lesen und Differenz berechnen */
    uint32_t remaining    = apic_read(APIC_TIMER_CCR);
    uint32_t elapsed      = 0xFFFFFFFFu - remaining;
    uint32_t ticks_per_ms = elapsed / CALIB_MS;
    timer_hz              = ticks_per_ms * 1000u;

    KINFO("APIC: kalibriert %u Ticks/ms  → %u Hz\n",
          ticks_per_ms, timer_hz);

    /* ── APIC-Timer scharf stellen ───────────────────────────── */
    /* LVT: periodischer Modus, Vektor 0x20 (IRQ0), 1 ms Intervall */
    apic_write(APIC_LVT_TIMER, LVT_TIMER_PERIODIC | LVT_TIMER_VECTOR);
    apic_write(APIC_TIMER_ICR, ticks_per_ms);

    apic_ok = true;
    KINFO("APIC: Local APIC ID=%u aktiv  Timer=%u Hz\n",
          apic_id(), timer_hz);
}

/* ── Öffentlich: apic_eoi ────────────────────────────────────── */
void apic_eoi(void)
{
    if (apic) apic_write(APIC_EOI_REG, 0);
}

uint32_t apic_timer_hz(void) { return timer_hz; }

uint8_t apic_id(void)
{
    if (!apic) return 0;
    return (uint8_t)(apic_read(APIC_ID) >> 24);
}

void apic_dump(void)
{
    char buf[80];
    snprintf(buf, sizeof(buf),
             "[APIC] ID=%u  aktiv=%s  Timer=%u Hz\n",
             apic_id(), apic_ok ? "ja" : "nein", timer_hz);
    serial_puts(buf);
}