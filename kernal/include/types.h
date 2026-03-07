/**
 * @file types.h
 * @brief Fundamental fixed-width type definitions for PinguinOS.
 *
 * The kernel is built freestanding (-ffreestanding) so we cannot rely on
 * <stdint.h> being available from a host libc. These definitions mirror the
 * standard ones and are safe for both i686 and x86_64 cross-compilers.
 */
#ifndef _TYPES_H
#define _TYPES_H

/* ── Unsigned integers ───────────────────────────────────────────── */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

/* ── Signed integers ─────────────────────────────────────────────── */
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

/* ── Pointer-sized integers ──────────────────────────────────────── */
typedef uint32_t            uintptr_t;   /* Change to uint64_t for 64-bit */
typedef int32_t             intptr_t;
typedef uint32_t            size_t;
typedef int32_t             ssize_t;

/* ── Boolean ─────────────────────────────────────────────────────── */
typedef uint8_t bool;
#define true  1
#define false 0

/* ── NULL ────────────────────────────────────────────────────────── */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* ── Useful macros ───────────────────────────────────────────────── */
#define ALIGN_UP(val, align)   (((val) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(val, align) ((val) & ~((align) - 1))
#define MIN(a, b)              ((a) < (b) ? (a) : (b))
#define MAX(a, b)              ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(arr)        (sizeof(arr) / sizeof((arr)[0]))
#define BIT(n)                 (1U << (n))

/* ── Compiler hints ──────────────────────────────────────────────── */
#define UNUSED(x)   ((void)(x))
#define PACKED      __attribute__((packed))
#define NORETURN    __attribute__((noreturn))
#define INLINE      __attribute__((always_inline)) inline
#define NOINLINE    __attribute__((noinline))

#endif /* _TYPES_H */
