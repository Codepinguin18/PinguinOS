/**
 * @file watchdog.c
 * @brief Watchdog timer for PinguinOS.
 *
 * Feature #99 – Watchdog Timer
 *
 * Uses the RTC periodic interrupt (IRQ8, ~64 Hz via rate selection)
 * as a heartbeat independent from the scheduler PIT.
 *
 * A dedicated watchdog task can also call watchdog_pet() every second;
 * both paths are supported.
 */

#include "../include/watchdog.h"
#include "../include/idt.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/acpi.h"   /* acpi_reboot() */

/* ── RTC CMOS ports ──────────────────────────────────────────────── */
#define CMOS_ADDR  0x70
#define CMOS_DATA  0x71
#define CMOS_REG_A 0x0A
#define CMOS_REG_B 0x0B
#define CMOS_REG_C 0x0C

/* ── Module state ────────────────────────────────────────────────── */
/* Rate 6 → 976.5625 µs period → ~1024 ticks/sec.
 * We count in whole seconds using a tick accumulator. */
#define WDT_RATE        6        /* RTC rate register bits  */
#define WDT_TICKS_PER_S 1024     /* Approximate tick rate   */

static volatile uint32_t wdt_ticks     = 0;
static volatile uint32_t wdt_pet_ticks = 0;   /* Value of wdt_ticks at last pet() */
static uint32_t          wdt_timeout_ticks = 0;
static bool              wdt_active    = false;

/* ── Action on timeout ───────────────────────────────────────────── */
static void watchdog_fire(void)
{
    cli();
    serial_puts("\n[WATCHDOG] TIMEOUT – system hung! Rebooting...\n");
    /* Hard reboot via ACPI or keyboard controller */
    acpi_reboot();
    /* If ACPI reboot fails, triple-fault via invalid IDT load */
    __asm__ volatile ("lidt (%0)" :: "r"(0));
    __asm__ volatile ("int $0x01");
    for (;;) hlt();
}

/* ── RTC IRQ8 handler ────────────────────────────────────────────── */
static void rtc_wdt_handler(cpu_regs_t *regs)
{
    (void)regs;

    /* Must read register C to clear the interrupt */
    outb(CMOS_ADDR, 0x8C);   /* NMI disabled | register C */
    inb(CMOS_DATA);

    if (!wdt_active) return;

    wdt_ticks++;

    uint32_t elapsed = wdt_ticks - wdt_pet_ticks;
    if (elapsed >= wdt_timeout_ticks)
        watchdog_fire();
}

/* ── Public: watchdog_init ───────────────────────────────────────── */
void watchdog_init(uint32_t timeout_seconds)
{
    if (timeout_seconds == 0)  timeout_seconds = 5;
    if (timeout_seconds > 60)  timeout_seconds = 60;

    wdt_timeout_ticks = timeout_seconds * WDT_TICKS_PER_S;

    /* Configure RTC periodic interrupt rate */
    uint32_t flags = irq_save();
    outb(CMOS_ADDR, 0x8A);           /* NMI disable, select reg A */
    uint8_t prev = inb(CMOS_DATA);
    outb(CMOS_ADDR, 0x8A);
    outb(CMOS_DATA, (prev & 0xF0) | WDT_RATE);

    /* Enable periodic interrupt in register B */
    outb(CMOS_ADDR, 0x8B);
    prev = inb(CMOS_DATA);
    outb(CMOS_ADDR, 0x8B);
    outb(CMOS_DATA, prev | 0x40);    /* PIE bit */

    /* Clear any pending interrupt */
    outb(CMOS_ADDR, 0x8C);
    inb(CMOS_DATA);
    irq_restore(flags);

    /* Register IRQ8 handler */
    irq_register(8, rtc_wdt_handler);

    KINFO("Watchdog: initialised  timeout=%u s  rate=%u Hz\n",
          timeout_seconds, WDT_TICKS_PER_S);
}

/* ── Public: watchdog_pet ────────────────────────────────────────── */
void watchdog_pet(void)
{
    wdt_pet_ticks = wdt_ticks;
}

/* ── Public: watchdog_enable / disable ───────────────────────────── */
void watchdog_enable(void)
{
    wdt_pet_ticks = wdt_ticks;
    wdt_active    = true;
    KINFO("Watchdog: enabled\n");
}

void watchdog_disable(void)
{
    wdt_active = false;
    KINFO("Watchdog: disabled\n");
}

bool watchdog_enabled(void) { return wdt_active; }