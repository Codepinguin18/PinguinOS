/**
 * @file entropy.h
 * @brief Entropy pool and pseudo-random number generator API – PinguinOS.
 *
 * Feature #100 – Entropy Pool / RNG
 *
 * Hardware events (keyboard, timer jitter, mouse movement) are mixed into
 * a 32-word pool.  The pool is stirred with a Lehmer-based LFSR.
 * rand_u32() drains the pool via XOR folding.
 */
#ifndef _ENTROPY_H
#define _ENTROPY_H

#include "types.h"

/* ── Public API ──────────────────────────────────────────────────── */

/** Initialise the entropy pool (seed with compile-time constant). */
void entropy_init(void);

/**
 * @brief Mix a hardware-derived value into the entropy pool.
 *
 * Call this from IRQ handlers, keyboard callbacks, mouse events, etc.
 * The more diverse the call sites the better the entropy.
 *
 * @param value  Any unpredictable 32-bit value (timestamp, scancode…).
 */
void entropy_add(uint32_t value);

/**
 * @brief Produce a 32-bit pseudo-random number.
 *
 * Folds the entire pool with XOR and advances the internal LFSR state.
 */
uint32_t rand_u32(void);

/**
 * @brief Produce a 32-bit number in the range [0, max).
 * @param max  Upper bound (exclusive).  Must be > 0.
 */
uint32_t rand_range(uint32_t max);

/**
 * @brief Fill a buffer with pseudo-random bytes.
 * @param buf  Destination buffer.
 * @param len  Number of bytes to fill.
 */
void rand_bytes(void *buf, size_t len);

#endif /* _ENTROPY_H */