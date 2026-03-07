/**
 * @file entropy.c
 * @brief Entropy pool and pseudo-random number generator for PinguinOS.
 *
 * Feature #100 – Entropy Pool / RNG
 *
 * Pool mixing uses a 32-word circular XOR, combined with a
 * 32-bit Galois LFSR for the output stage.  This is not cryptographically
 * secure but is more than sufficient for kernel jitter, scheduling
 * randomisation, and network transaction IDs.
 */

#include "../include/entropy.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"   /* sched_uptime_ms() */

/* ── Pool ────────────────────────────────────────────────────────── */
#define POOL_WORDS  32

static uint32_t pool[POOL_WORDS];
static uint32_t pool_pos = 0;

/* ── Galois LFSR state ───────────────────────────────────────────── */
/* Primitive polynomial: x^32 + x^22 + x^2 + x + 1  (taps at 32,22,2,1) */
#define LFSR_POLY  0xB4BCD35Cu

static uint32_t lfsr_state = 0xDEAD1337u;   /* Non-zero seed */

static uint32_t lfsr_next(void)
{
    uint32_t lsb = lfsr_state & 1u;
    lfsr_state >>= 1;
    if (lsb) lfsr_state ^= LFSR_POLY;
    return lfsr_state;
}

/* ── Public: entropy_init ────────────────────────────────────────── */
void entropy_init(void)
{
    /* Seed the pool with diverse compile-time and runtime values */
    pool[0]  = 0xCAFEBABEu;
    pool[1]  = 0x31415926u;
    pool[2]  = 0xABCDEF01u;
    pool[3]  = 0x87654321u;

    for (int i = 4; i < POOL_WORDS; i++)
        pool[i] = lfsr_next() ^ (uint32_t)(i * 0x9E3779B9u);

    KINFO("Entropy: pool initialised (%u words)\n", POOL_WORDS);
}

/* ── Public: entropy_add ─────────────────────────────────────────── */
/**
 * We mix the incoming value using a SipHash-like round:
 *   – XOR with the uptime (adds timing jitter)
 *   – XOR into a pool slot (circular)
 *   – Rotate and XOR the adjacent slot (diffuse)
 */
void entropy_add(uint32_t value)
{
    uint32_t t = sched_uptime_ms();
    value ^= t ^ lfsr_next();

    uint32_t idx = pool_pos % POOL_WORDS;
    pool[idx] ^= value;

    /* Diffuse into the next slot */
    uint32_t next_idx = (idx + 1) % POOL_WORDS;
    pool[next_idx] ^= ((value << 7) | (value >> 25));   /* rol32 7 */

    pool_pos++;
}

/* ── Public: rand_u32 ────────────────────────────────────────────── */
uint32_t rand_u32(void)
{
    /* Fold the entire pool with XOR */
    uint32_t result = lfsr_next();
    for (int i = 0; i < POOL_WORDS; i++)
        result ^= pool[i];

    /* Advance pool state to ensure different output on next call */
    pool[pool_pos % POOL_WORDS] ^= result;
    pool_pos++;

    /* Final mixing: multiply-xorshift */
    result ^= result >> 16;
    result *= 0x45D9F3Bu;
    result ^= result >> 16;

    return result;
}

/* ── Public: rand_range ──────────────────────────────────────────── */
uint32_t rand_range(uint32_t max)
{
    if (max <= 1) return 0;
    /* Rejection sampling to avoid modulo bias */
    uint32_t threshold = (~max + 1u) % max;
    uint32_t r;
    do { r = rand_u32(); } while (r < threshold);
    return r % max;
}

/* ── Public: rand_bytes ──────────────────────────────────────────── */
void rand_bytes(void *buf, size_t len)
{
    uint8_t *out = (uint8_t *)buf;
    while (len >= 4) {
        uint32_t r = rand_u32();
        memcpy(out, &r, 4);
        out += 4;
        len -= 4;
    }
    if (len) {
        uint32_t r = rand_u32();
        memcpy(out, &r, len);
    }
}