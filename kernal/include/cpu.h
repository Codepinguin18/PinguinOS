/**
 * @file cpu.h
 * @brief CPU identification and control helpers for PinguinOS.
 *
 * Wraps CPUID, MSR access, and common inline assembly helpers
 * (cli/sti, hlt, io port read/write).
 */
#ifndef _CPU_H
#define _CPU_H

#include "types.h"

/* ── CPUID leaves ────────────────────────────────────────────────── */
#define CPUID_VENDOR    0x00000000
#define CPUID_FEATURES  0x00000001
#define CPUID_BRAND1    0x80000002
#define CPUID_BRAND2    0x80000003
#define CPUID_BRAND3    0x80000004

/* ── CPUID feature bits (EDX, leaf 1) ───────────────────────────── */
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

/* ── CPU info structure ──────────────────────────────────────────── */
typedef struct cpu_info {
    char     vendor[13];      /* Null-terminated vendor string         */
    char     brand[49];       /* Null-terminated brand/model string    */
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint32_t features_edx;    /* CPUID leaf 1 EDX feature flags        */
    uint32_t features_ecx;    /* CPUID leaf 1 ECX feature flags        */
    bool     has_cpuid;       /* false on very old CPUs                */
} cpu_info_t;

/* ── Public API ──────────────────────────────────────────────────── */

/** Populate the global cpu_info structure. */
void cpu_init(void);

/** @return Pointer to the populated cpu_info structure. */
const cpu_info_t *cpu_get_info(void);

/** Print CPU info to the serial log. */
void cpu_dump(void);

/* ── I/O port helpers ────────────────────────────────────────────── */

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

/** Small I/O delay (used after slow legacy port accesses). */
static INLINE void io_wait(void) {
    outb(0x80, 0x00);
}

/* ── Interrupt control ───────────────────────────────────────────── */

static INLINE void sti(void)  { __asm__ volatile ("sti"); }
static INLINE void cli(void)  { __asm__ volatile ("cli"); }
static INLINE void hlt(void)  { __asm__ volatile ("hlt"); }
static INLINE void pause(void){ __asm__ volatile ("pause"); }

/** Save current interrupt flag and disable interrupts. */
static INLINE uint32_t irq_save(void) {
    uint32_t flags;
    __asm__ volatile ("pushf; pop %0; cli" : "=r"(flags));
    return flags;
}

/** Restore interrupt flag saved by irq_save(). */
static INLINE void irq_restore(uint32_t flags) {
    __asm__ volatile ("push %0; popf" : : "r"(flags) : "cc");
}

/* ── Memory barriers ─────────────────────────────────────────────── */
static INLINE void mb(void)  { __asm__ volatile ("mfence" ::: "memory"); }
static INLINE void rmb(void) { __asm__ volatile ("lfence" ::: "memory"); }
static INLINE void wmb(void) { __asm__ volatile ("sfence" ::: "memory"); }

/* ── CPUID helper ────────────────────────────────────────────────── */
static INLINE void cpuid(uint32_t leaf,
                          uint32_t *eax, uint32_t *ebx,
                          uint32_t *ecx, uint32_t *edx)
{
    __asm__ volatile ("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "0"(leaf));
}

/* ── CR register access ──────────────────────────────────────────── */
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
static INLINE uint32_t read_cr4(void) {
    uint32_t v; __asm__ volatile ("mov %%cr4, %0" : "=r"(v)); return v;
}
static INLINE void write_cr4(uint32_t v) {
    __asm__ volatile ("mov %0, %%cr4" :: "r"(v));
}

/* ── Kernel panic ────────────────────────────────────────────────── */
/**
 * @brief Print a panic message and halt the CPU.
 *
 * This function does not return.  Interrupts are disabled before halting
 * so we do not accidentally handle another IRQ after the panic.
 */
void NORETURN kpanic(const char *fmt, ...);

#define KASSERT(cond) \
    do { if (!(cond)) kpanic("Assertion failed: %s  (%s:%d)", \
                              #cond, __FILE__, __LINE__); } while(0)

#endif /* _CPU_H */
