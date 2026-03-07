/**
 * @file watchdog.h + watchdog.c  (combined – split by the build system)
 * @brief Watchdog timer for PinguinOS using the RTC periodic interrupt.
 *
 * Feature #99 – Watchdog Timer
 *
 * The watchdog fires after a configurable timeout (default 5 s).
 * A kernel task must call watchdog_pet() before the deadline or the
 * system is considered hung and watchdog_fire() triggers a reboot.
 *
 * Implementation uses the RTC periodic interrupt (IRQ8) as the time
 * base so it is independent of the PIT / scheduler.
 */
#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include "types.h"

/** Initialise the watchdog with a timeout of @p seconds (1–60). */
void watchdog_init(uint32_t timeout_seconds);

/** Reset the watchdog countdown – must be called periodically. */
void watchdog_pet(void);

/** Enable the watchdog (starts counting). */
void watchdog_enable(void);

/** Disable the watchdog (stop counting). */
void watchdog_disable(void);

/** @return true if the watchdog is currently enabled. */
bool watchdog_enabled(void);

#endif /* _WATCHDOG_H */