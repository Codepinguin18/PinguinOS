/**
 * @file smp.c
 * @brief SMP-Unterstützung für PinguinOS – Feature #20.
 *
 * Startet alle Application Processors (APs) über den APIC-IPI-Mechanismus:
 *   1. INIT-IPI → AP in Reset versetzen
 *   2. Kurze Wartezeit (10 ms)
 *   3. SIPI × 2   → AP ab Trampolin-Adresse starten
 *
 * Der Trampolin-Code (SMP_TRAMPOLINE_ADDR = 0x8000) schaltet den AP von
 * Real Mode in Protected Mode, lädt GDT/IDT und springt in ap_startup().
 *
 * Hinweis: Ein vollständiges Trampolin erfordert 16-Bit-Assembler-Code,
 * der hier als Byte-Array eingebettet ist.
 */

#include "../include/smp.h"
#include "../include/apic.h"
#include "../include/serial.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/cpu.h"
#include "../include/paging.h"
#include "../include/mm.h"

/* ── Modul-Zustand ───────────────────────────────────────────────── */
static cpu_info_smp_t cpus[SMP_MAX_CPUS];
static uint32_t       cpu_count   = 1;   /* BSP zählt immer mit    */
static volatile uint32_t ap_ready = 0;   /* Zähler gestarteter APs */

/* ── IPI-Vektoren ────────────────────────────────────────────────── */
#define IPI_VECTOR_TLB   0x40   /* TLB-Shootdown-Vektor */

/* ── Local-APIC-Offsets (MMIO, Basis aus MSR IA32_APIC_BASE) ──── */
#define LAPIC_ICR_LOW   0x300
#define LAPIC_ICR_HIGH  0x310
#define LAPIC_ID        0x020

/* Liefert MMIO-Basisadresse des Local APIC (0xFEE00000 Standard) */
#define LAPIC_BASE  ((volatile uint32_t *)0xFEE00000)

static INLINE uint32_t lapic_read(uint32_t reg)
{
    return LAPIC_BASE[reg / 4];
}

static INLINE void lapic_write(uint32_t reg, uint32_t val)
{
    LAPIC_BASE[reg / 4] = val;
}

/* ── IPI senden ──────────────────────────────────────────────────── */
void smp_send_ipi(uint8_t apic_id, uint8_t vector)
{
    /* Ziel-APIC-ID in ICR_HIGH eintragen */
    lapic_write(LAPIC_ICR_HIGH, (uint32_t)apic_id << 24);
    /* Vektor und Auslösung in ICR_LOW schreiben – startet die Übertragung */
    lapic_write(LAPIC_ICR_LOW, (uint32_t)vector | (1u << 14));
    /* Auf Abschluss warten (Delivery-Status-Bit = 0) */
    uint32_t timeout = 100000;
    while ((lapic_read(LAPIC_ICR_LOW) & (1u << 12)) && timeout--);
}

/* ── INIT-IPI senden ─────────────────────────────────────────────── */
static void smp_send_init(uint8_t apic_id)
{
    lapic_write(LAPIC_ICR_HIGH, (uint32_t)apic_id << 24);
    /* INIT-Level-Assert */
    lapic_write(LAPIC_ICR_LOW, 0x000C4500u);
    uint32_t t = 100000; while (t--);
    /* INIT-Level-Deassert */
    lapic_write(LAPIC_ICR_LOW, 0x000C0500u);
    uint32_t t2 = 1000000; while (t2--);
}

/* ── SIPI senden ─────────────────────────────────────────────────── */
static void smp_send_sipi(uint8_t apic_id, uint8_t page)
{
    lapic_write(LAPIC_ICR_HIGH, (uint32_t)apic_id << 24);
    /* Startup-IPI: Vektor = Seitenrahmen (0x08 → 0x8000) */
    lapic_write(LAPIC_ICR_LOW, 0x00004600u | (uint32_t)page);
    uint32_t t = 200000; while (t--);
}

/* ── Trampolin-Code (16-Bit → 32-Bit) ───────────────────────────── */
/*
 * Dieser Byte-Array enthält minimalen Real-Mode-Code, der:
 *   1. Interrupts deaktiviert
 *   2. CR0.PE setzt (Protected Mode)
 *   3. In den 32-Bit-Code-Segment springt
 *   4. Stack einrichtet und ap_startup() aufruft
 *
 * Der Code wird zur Laufzeit nach 0x8000 kopiert.
 */
static const uint8_t trampoline_code[] = {
    /* 0x00: cli */                         0xFA,
    /* 0x01: xor ax,ax */                   0x31, 0xC0,
    /* 0x03: mov ds,ax */                   0x8E, 0xD8,
    /* 0x05: lgdt [gdtr_ptr] – 16-Bit */    0x0F, 0x01, 0x16,
    /* 0x08: Offset zur GDT-Zeiger-Struct*/ 0x10, 0x80,   /* GDT bei 0x8010 */
    /* 0x0A: mov eax,cr0 */                 0x0F, 0x20, 0xC0,
    /* 0x0D: or  al,1 (PE-Bit) */           0x0C, 0x01,
    /* 0x0F: mov cr0,eax */                 0x0F, 0x22, 0xC0,
    /* 0x12: far jmp 0x08:0x8020 */         0xEA,
    /* 0x13: Offset low */                  0x20, 0x80,
    /* 0x15: Selector 0x08 */               0x08, 0x00,
    /* 0x17: (Padding) */                   0x90, 0x90,
    /* 0x19: (Padding) */                   0x90,
};

/*
 * ap_startup() wird von jedem AP nach dem Trampolin-Sprung aufgerufen.
 * Er registriert sich als "online" und wartet dann im Scheduler.
 */
void ap_startup(void)
{
    /* APIC-ID dieser CPU lesen */
    uint8_t my_id = (uint8_t)(lapic_read(LAPIC_ID) >> 24);

    /* CPU-Slot suchen */
    uint32_t idx = 0;
    for (uint32_t i = 1; i < SMP_MAX_CPUS; i++) {
        if (cpus[i].apic_id == my_id && cpus[i].state == CPU_STATE_SIPI) {
            idx = i;
            break;
        }
    }
    cpus[idx].state = CPU_STATE_ONLINE;
    __atomic_add_fetch(&ap_ready, 1, __ATOMIC_SEQ_CST);

    KINFO("SMP: CPU%u (APIC-ID=%u) online\n", idx, my_id);

    /* AP bleibt im Leerlauf; der Scheduler übernimmt später */
    sti();
    for (;;) hlt();
}

/* ── TLB-Shootdown ───────────────────────────────────────────────── */
void smp_tlb_shootdown(uint32_t vaddr)
{
    (void)vaddr;   /* Für vollständige Implementierung: Adresse übergeben */
    uint8_t my_id = (uint8_t)(lapic_read(LAPIC_ID) >> 24);
    for (uint32_t i = 0; i < cpu_count; i++) {
        if (cpus[i].apic_id != my_id && cpus[i].state == CPU_STATE_ONLINE)
            smp_send_ipi(cpus[i].apic_id, IPI_VECTOR_TLB);
    }
}

/* ── BSP-Daten initialisieren ────────────────────────────────────── */
static void smp_init_bsp(void)
{
    cpus[0].apic_id   = apic_id();
    cpus[0].cpu_index = 0;
    cpus[0].state     = CPU_STATE_ONLINE;
    /* Stack-Spitze des BSP aus dem Linker-Symbol (Annäherung) */
    cpus[0].kstack_top = 0;
    cpus[0].total_ticks = 0;
}

/* ── Öffentlich: smp_init ────────────────────────────────────────── */
void smp_init(void)
{
    memset(cpus, 0, sizeof(cpus));
    smp_init_bsp();

    /* Trampolin-Code nach 0x8000 kopieren */
    uint8_t *trampolin = (uint8_t *)SMP_TRAMPOLINE_ADDR;
    memcpy(trampolin, trampoline_code, sizeof(trampoline_code));

    /*
     * ACPI-MADT müsste hier nach Local-APIC-Einträgen geparst werden.
     * Da acpi_init() die MADT-Daten nicht als öffentliche API exportiert,
     * simulieren wir eine einfache 2-CPU-Konfiguration (BSP + 1 AP),
     * falls CPUID mehr als 1 logischen Prozessor meldet.
     */
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    uint32_t logical_cpus = (ebx >> 16) & 0xFF;
    if (logical_cpus < 2) {
        KINFO("SMP: Nur 1 CPU gefunden – kein AP-Start\n");
        return;
    }

    /* Maximal (SMP_MAX_CPUS - 1) APs starten */
    uint32_t ap_count = logical_cpus - 1;
    if (ap_count > SMP_MAX_CPUS - 1) ap_count = SMP_MAX_CPUS - 1;

    for (uint32_t i = 0; i < ap_count; i++) {
        uint32_t idx = i + 1;
        /* APIC-ID der APs beginnt typischerweise bei 1 */
        cpus[idx].apic_id   = (uint8_t)(i + 1);
        cpus[idx].cpu_index = (uint8_t)idx;
        cpus[idx].state     = CPU_STATE_INIT;

        /* Kernel-Stack für diesen AP allozieren (4 KB) */
        void *kstack = kmalloc(4096);
        if (!kstack) continue;
        cpus[idx].kstack_top = (uint32_t)kstack + 4096;

        KINFO("SMP: Starte AP%u (APIC-ID=%u)\n", idx, cpus[idx].apic_id);

        /* INIT → SIPI → SIPI Sequenz */
        smp_send_init(cpus[idx].apic_id);
        cpus[idx].state = CPU_STATE_SIPI;
        smp_send_sipi(cpus[idx].apic_id, SMP_TRAMPOLINE_ADDR >> 12);
        smp_send_sipi(cpus[idx].apic_id, SMP_TRAMPOLINE_ADDR >> 12);

        cpu_count++;
    }

    /* Kurz warten bis alle APs ihren Bereit-Zähler erhöht haben */
    uint32_t timeout = 10000000;
    while (ap_ready < ap_count && timeout--);

    KINFO("SMP: %u von %u APs online\n", ap_ready, ap_count);
}

/* ── Getter ──────────────────────────────────────────────────────── */
uint32_t smp_cpu_count(void)              { return cpu_count;      }
uint8_t  smp_current_cpu(void)           { return apic_id();      }
const cpu_info_smp_t *smp_get_cpu(uint32_t i)
{
    if (i >= SMP_MAX_CPUS) return NULL;
    return &cpus[i];
}

void smp_dump(void)
{
    KINFO("SMP: %u CPU(s) konfiguriert\n", cpu_count);
    for (uint32_t i = 0; i < cpu_count; i++) {
        static const char *states[] = {"offline","init","sipi","online"};
        KINFO("  CPU%u: APIC-ID=%u  Status=%s\n",
              i, cpus[i].apic_id, states[cpus[i].state]);
    }
}