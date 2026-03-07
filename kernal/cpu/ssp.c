/**
 * @file ssp.c
 * @brief Stack-Smashing Protection (SSP) runtime for PinguinOS.
 *
 * Feature #95 – Stack-Smashing Protection
 *
 * GCC's -fstack-protector inserts a "canary" value onto the stack at
 * function entry and checks it on return.  When the canary has been
 * corrupted, it calls __stack_chk_fail().
 *
 * This file provides:
 *   __stack_chk_guard  – the canary value (randomised at boot)
 *   __stack_chk_fail   – the failure handler (calls kpanic + backtrace)
 *
 * Build flags to enable in Makefile:
 *   CFLAGS += -fstack-protector-strong
 *   (remove -fno-stack-protector if present)
 */

#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/entropy.h"
#include "../include/klib.h"

/* ── Canary value ────────────────────────────────────────────────── */
/*
 * GCC expects __stack_chk_guard to be a global.
 * On x86-32 it is 4 bytes wide.  We seed it with rand_u32() during
 * ssp_init() so it is different every boot.
 */
uintptr_t __stack_chk_guard = 0xDEADC0DEu;

/* ── ssp_init: randomise the canary ─────────────────────────────── */
/**
 * @brief Randomise the stack canary.
 *
 * Must be called after entropy_init() so the PRNG has been seeded.
 * Call this early in cmain(), before any -fstack-protector functions
 * might return (i.e. right after entropy_init).
 */
void ssp_init(void)
{
    /* Canary must never be 0x00000000 (string-copy terminator) */
    uint32_t canary;
    do { canary = rand_u32(); } while (canary == 0);
    __stack_chk_guard = canary;

    KINFO("SSP: stack canary = 0x%08x\n", (uint32_t)__stack_chk_guard);
}

/* ── __stack_chk_fail ────────────────────────────────────────────── */
/**
 * @brief Called by GCC when a stack corruption is detected.
 *
 * This function must be NORETURN.  We call kpanic() which already
 * invokes print_backtrace() (Feature #92) so we get a full trace.
 *
 * The __attribute__((noreturn)) tells GCC this never returns, which
 * avoids an infinite recursion if kpanic itself is protected.
 */
__attribute__((noreturn))
void __stack_chk_fail(void)
{
    /* Disable interrupts immediately – the stack is corrupt */
    cli();
    serial_puts("\n[SSP] *** STACK SMASHING DETECTED ***\n");
    kpanic("Stack-Smashing Protection triggered – canary overwritten!\n");
    /* kpanic() never returns */
    for (;;) hlt();
}