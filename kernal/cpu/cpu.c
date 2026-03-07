/**
 * @file cpu.c
 * @brief CPU-Identifizierung und Implementierung von Kernel Panic.
 */

#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/vga.h"
#include "../include/klib.h"

/* ── Modul-private Daten ─────────────────────────────────────────── */
static cpu_info_t cpu_info;

/* ── Helfer: Existiert CPUID? ────────────────────────────────────── */
static bool cpuid_supported(void)
{
    /*
     * CPUID ist verfügbar, wenn das ID-Flag (Bit 21) in den EFLAGS
     * umgeschaltet werden kann. 486er und frühere CPUs besitzen kein CPUID.
     */
    uint32_t before, after;
    __asm__ volatile (
        "pushf\n\t"
        "pop  %0\n\t"
        "mov  %0, %1\n\t"
        "xor  $0x00200000, %0\n\t"
        "push %0\n\t"
        "popf\n\t"
        "pushf\n\t"
        "pop  %0\n\t"
        : "=r"(after), "=r"(before)
        :
        : "cc"
    );
    return (before ^ after) & 0x00200000;
}

/* ── Öffentlich: cpu_init ────────────────────────────────────────── */
void cpu_init(void)
{
    memset(&cpu_info, 0, sizeof(cpu_info));

    cpu_info.has_cpuid = cpuid_supported();
    if (!cpu_info.has_cpuid) {
        memcpy(cpu_info.vendor, "Unbekannt", 10);
        return;
    }

    uint32_t eax, ebx, ecx, edx;

    /* Leaf 0: Vendor-String */
    cpuid(CPUID_VENDOR, &eax, &ebx, &ecx, &edx);
    /* Der Vendor-String ist gepackt in EBX, EDX, ECX (in dieser Reihenfolge) */
    memcpy(cpu_info.vendor + 0, &ebx, 4);
    memcpy(cpu_info.vendor + 4, &edx, 4);
    memcpy(cpu_info.vendor + 8, &ecx, 4);
    cpu_info.vendor[12] = '\0';

    /* Leaf 1: Family/Model/Stepping + Feature-Flags */
    cpuid(CPUID_FEATURES, &eax, &ebx, &ecx, &edx);
    cpu_info.stepping      = eax & 0xF;
    cpu_info.model         = (eax >> 4)  & 0xF;
    cpu_info.family        = (eax >> 8)  & 0xF;
    cpu_info.features_edx  = edx;
    cpu_info.features_ecx  = ecx;

    /* Erweiterte Leaves: Brand-String (falls verfügbar) */
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    if (eax >= CPUID_BRAND3) {
        uint32_t brand[12];
        cpuid(CPUID_BRAND1, &brand[0],  &brand[1],  &brand[2],  &brand[3]);
        cpuid(CPUID_BRAND2, &brand[4],  &brand[5],  &brand[6],  &brand[7]);
        cpuid(CPUID_BRAND3, &brand[8],  &brand[9],  &brand[10], &brand[11]);
        memcpy(cpu_info.brand, brand, 48);
        cpu_info.brand[48] = '\0';
    } else {
        memcpy(cpu_info.brand, cpu_info.vendor, 12);
        cpu_info.brand[12] = '\0';
    }
}

/* ── Öffentlich: Zugriffsmethoden ────────────────────────────────── */
const cpu_info_t *cpu_get_info(void) { return &cpu_info; }

void cpu_dump(void)
{
    KINFO("CPU Vendor  : %s\n", cpu_info.vendor);
    KINFO("CPU Brand   : %s\n", cpu_info.brand);
    KINFO("CPU Family  : %u  Model %u  Stepping %u\n",
          cpu_info.family, cpu_info.model, cpu_info.stepping);
    KINFO("CPU Features: EDX=0x%08x  ECX=0x%08x\n",
          cpu_info.features_edx, cpu_info.features_ecx);
}

/* ── Öffentlich: kpanic ──────────────────────────────────────────── */
void kpanic(const char *fmt, ...)
{
    cli();   /* Interrupts deaktivieren – wir sind am Ende */

    /* Ausgabe auf VGA (Bildschirm) und Serial (QEMU-Log) */
    vga_set_color(VGA_WHITE, VGA_RED);
    vga_puts("\n\n  *** KERNEL PANIC ***\n  ");

    char buf[512];
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    __builtin_va_end(ap);

    vga_puts(buf);
    serial_puts("\n[PANIC] ");
    serial_puts(buf);
    serial_puts("\n");

    /* Dauerhafter Halt */
    for (;;) hlt();
}
