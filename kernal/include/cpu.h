/**
 * @file cpu.h
 * @brief CPU-Identifikations- und Steuerungs-Helfer für PinguinOS.
 *
 * Beinhaltet CPUID, MSR-Zugriff und gängige Inline-Assembler-Helfer
 * (cli/sti, hlt, I/O Port Lese-/Schreibzugriffe).
 */
#ifndef _CPU_H
#define _CPU_H

#include "types.h"

/* ── CPUID Blätter ───────────────────────────────────────────────── */
#define CPUID_VENDOR    0x00000000
#define CPUID_FEATURES  0x00000001
#define CPUID_BRAND1    0x80000002
#define CPUID_BRAND2    0x80000003
#define CPUID_BRAND3    0x80000004

/* ── CPUID Feature-Bits (EDX, Blatt 1) ───────────────────────────── */
#define CPUID_FPU    (1 <<  0)
#define CPUID_VME    (1 <<  1)
#define CPUID_DE     (1 <<  2)
#define CPUID_PSE    (1 <<  3)
#define CPUID_TSC    (1 <<  4)
#define CPUID_MSR    (1 <<  5)
#define CPUID_PAE    (1 <<  6)
#define CPUID_APIC   (1 <<  9)
#define CPUID_SEP    (1 << 11)
#define CPUID_MTRR   (1 << 12)
#define CPUID_PGE    (1 << 13)
#define CPUID_CMOV   (1 << 15)
#define CPUID_MMX    (1 << 23)
#define CPUID_FXSR   (1 << 24)
#define CPUID_SSE    (1 << 25)
#define CPUID_SSE2   (1 << 26)

/* ── CPU-Info-Struktur ───────────────────────────────────────────── */
typedef struct cpu_info {
    char     vendor[13];      /* Null-terminierter Vendor-String       */
    char     brand[49];       /* Null-terminierter Brand/Modell-String */
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint32_t features_edx;    /* CPUID Blatt 1 EDX Feature-Flags       */
    uint32_t features_ecx;    /* CPUID Blatt 1 ECX Feature-Flags       */
    bool     has_cpuid;       /* false auf sehr alten CPUs             */
} cpu_info_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** Fügt Daten in die globale cpu_info-Struktur ein. */
void cpu_init(void);

/** @return Zeiger auf die befüllte cpu_info-Struktur. */
const cpu_info_t *cpu_get_info(void);

/** Gibt CPU-Informationen im Serial-Log aus. */
void cpu_dump(void);

/* ── I/O Port Helfer ─────────────────────────────────────────────── */

static INLINE void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static INLINE void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static INLINE void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static INLINE uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static INLINE uint16_t inw(uint16_t port) {
    uint16_t val;
    __asm__ volatile ("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static INLINE uint32_t inl(uint16_t port) {
    uint32_t val;
    __asm__ volatile ("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

/** Kleine I/O Verzögerung (verwendet nach langsamen Legacy-Port-Zugriffen). */
static INLINE void io_wait(void) {
    outb(0x80, 0x00);
}

/* ── Interrupt-Steuerung ─────────────────────────────────────────── */

static INLINE void sti(void)  { __asm__ volatile ("sti"); }
static INLINE void cli(void)  { __asm__ volatile ("cli"); }
static INLINE void hlt(void)  { __asm__ volatile ("hlt"); }
static INLINE void pause(void){ __asm__ volatile ("pause"); }

/** Aktuelle Interrupt-Flags speichern und Interrupts deaktivieren. */
static INLINE uint32_t irq_save(void) {
    uint32_t flags;
    __asm__ volatile ("pushf; pop %0; cli" : "=r"(flags));
    return flags;
}

/** Stellt durch irq_save() gespeicherte Interrupt-Flags wieder her. */
static INLINE void irq_restore(uint32_t flags) {
    __asm__ volatile ("push %0; popf" : : "r"(flags) : "cc");
}

/* ── Speicherbarrieren ────────────────────────────────────────────── */
static INLINE void mb(void)  { __asm__ volatile ("mfence" ::: "memory"); }
static INLINE void rmb(void) { __asm__ volatile ("lfence" ::: "memory"); }
static INLINE void wmb(void) { __asm__ volatile ("sfence" ::: "memory"); }

/* ── CPUID Helfer ────────────────────────────────────────────────── */
static INLINE void cpuid(uint32_t leaf,
                          uint32_t *eax, uint32_t *ebx,
                          uint32_t *ecx, uint32_t *edx)
{
    __asm__ volatile ("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "0"(leaf));
}

/* ── CR Register Zugriff ─────────────────────────────────────────── */
static INLINE uint32_t read_cr0(void) {
    uint32_t v; __asm__ volatile ("mov %%cr0, %0" : "=r"(v)); return v;
}
static INLINE void write_cr0(uint32_t v) {
    __asm__ volatile ("mov %0, %%cr0" :: "r"(v));
}
static INLINE uint32_t read_cr2(void) {
    uint32_t v; __asm__ volatile ("mov %%cr2, %0" : "=r"(v)); return v;
}
static INLINE uint32_t read_cr3(void) {
    uint32_t v; __asm__ volatile ("mov %%cr3, %0" : "=r"(v)); return v;
}
static INLINE void write_cr3(uint32_t v) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(v) : "memory");
}

/* ── Kernel Panic ────────────────────────────────────────────────── */
/**
 * @brief Gibt eine Panic-Meldung aus und hält die CPU an.
 *
 * Diese Funktion kehrt nicht zurück. Interrupts werden vor dem Anhalten
 * deaktiviert, um nicht versehentlich weitere IRQs nach der Panic zu verarbeiten.
 */
void NORETURN kpanic(const char *fmt, ...);

#define KASSERT(cond) \
    do { if (!(cond)) kpanic("Assertion failed: %s  (%s:%d)", \
                               #cond, __FILE__, __LINE__); } while(0)

#endif /* _CPU_H */
