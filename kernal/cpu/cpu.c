/**
 * @file cpu.c
 * @brief CPU identification and kernel panic implementation.
 *
 * Feature #92 – Backtrace:
 *   kpanic() now walks the EBP chain and prints the call stack to both
 *   VGA and the serial console.
 */

#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/vga.h"
#include "../include/klib.h"

/* ── Module-private data ─────────────────────────────────────────── */
static cpu_info_t cpu_info;

/* ── Helper: does CPUID exist? ───────────────────────────────────── */
static bool cpuid_supported(void)
{
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

/* ── Public: cpu_init ────────────────────────────────────────────── */
void cpu_init(void)
{
    memset(&cpu_info, 0, sizeof(cpu_info));

    cpu_info.has_cpuid = cpuid_supported();
    if (!cpu_info.has_cpuid) {
        memcpy(cpu_info.vendor, "Unknown", 8);
        return;
    }

    uint32_t eax, ebx, ecx, edx;

    cpuid(CPUID_VENDOR, &eax, &ebx, &ecx, &edx);
    memcpy(cpu_info.vendor + 0, &ebx, 4);
    memcpy(cpu_info.vendor + 4, &edx, 4);
    memcpy(cpu_info.vendor + 8, &ecx, 4);
    cpu_info.vendor[12] = '\0';

    cpuid(CPUID_FEATURES, &eax, &ebx, &ecx, &edx);
    cpu_info.stepping     = eax & 0xF;
    cpu_info.model        = (eax >> 4)  & 0xF;
    cpu_info.family       = (eax >> 8)  & 0xF;
    cpu_info.features_edx = edx;
    cpu_info.features_ecx = ecx;

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

/* ── Public: accessors ───────────────────────────────────────────── */
const cpu_info_t *cpu_get_info(void) { return &cpu_info; }

void cpu_dump(void)
{
    KINFO("CPU vendor  : %s\n", cpu_info.vendor);
    KINFO("CPU brand   : %s\n", cpu_info.brand);
    KINFO("CPU family  : %u  model %u  stepping %u\n",
          cpu_info.family, cpu_info.model, cpu_info.stepping);
    KINFO("CPU features: EDX=0x%08x  ECX=0x%08x\n",
          cpu_info.features_edx, cpu_info.features_ecx);
}

/* ── Feature #92: Stack Backtrace ────────────────────────────────── */
/**
 * @brief Walk the EBP chain and print return addresses.
 *
 * The frame layout on x86 with -fno-omit-frame-pointer is:
 *
 *   [EBP+0]  = previous EBP
 *   [EBP+4]  = return address (EIP of the caller)
 *
 * We stop when EBP is NULL, misaligned, or the EIP looks implausible
 * (i.e. below 0x100000 or above 0xE0000000).
 */
void print_backtrace(void)
{
    uint32_t *ebp;
    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp));

    serial_puts("[BACKTRACE] Stack trace:\n");
    vga_puts("[BACKTRACE]\n");

    char linebuf[64];

    for (int depth = 0; depth < 20; depth++) {
        /* Sanity-check EBP */
        if (!ebp) break;
        if ((uint32_t)ebp < 0x1000 || (uint32_t)ebp > 0xEFFFFFFC) break;
        if ((uint32_t)ebp & 3) break;   /* Must be 4-byte aligned */

        uint32_t ret_eip = ebp[1];

        /* Check return address plausibility */
        if (ret_eip < 0x100000 || ret_eip > 0xE0000000) break;

        snprintf(linebuf, sizeof(linebuf),
                 "  #%-2d  0x%08x\n", depth, ret_eip);
        serial_puts(linebuf);
        vga_puts(linebuf);

        ebp = (uint32_t *)ebp[0];
    }

    serial_puts("[BACKTRACE] End\n");
}

/* ── Public: kpanic ──────────────────────────────────────────────── */
void kpanic(const char *fmt, ...)
{
    cli();   /* Disable interrupts – we are done */

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

    /* Feature #92 – Print backtrace on every panic */
    print_backtrace();

    for (;;) hlt();
}