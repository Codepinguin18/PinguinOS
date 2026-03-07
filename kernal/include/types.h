/**
 * @file types.h
 * @brief Grundlegende Definitionen von Typen fester Breite für PinguinOS.
 *
 * Der Kernel wird eigenständig (-ffreestanding) gebaut, daher können wir uns nicht
 * darauf verlassen, dass <stdint.h> von einer Host-libc verfügbar ist. Diese
 * Definitionen spiegeln die Standardwerte wider und sind sicher für i686 und
 * x86_64 Cross-Compiler.
 */
#ifndef _TYPES_H
#define _TYPES_H

/* ── Vorzeichenlose Ganzzahlen ───────────────────────────────────── */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

/* ── Vorzeichenbehaftete Ganzzahlen ───────────────────────────────── */
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

/* ── Ganzzahlen in Pointer-Größe ──────────────────────────────────── */
typedef uint32_t            uintptr_t;   /* Für 64-Bit auf uint64_t ändern */
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

/* ── Nützliche Macros ─────────────────────────────────────────────── */
#define ALIGN_UP(val, align)   (((val) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(val, align) ((val) & ~((align) - 1))
#define MIN(a, b)              ((a) < (b) ? (a) : (b))
#define MAX(a, b)              ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(arr)        (sizeof(arr) / sizeof((arr)[0]))
#define BIT(n)                 (1U << (n))

/* ── Compiler-Hinweise ───────────────────────────────────────────── */
#define UNUSED(x)   ((void)(x))
#define PACKED      __attribute__((packed))
#define NORETURN    __attribute__((noreturn))
#define INLINE      __attribute__((always_inline)) inline
#define NOINLINE    __attribute__((noinline))

#endif /* _TYPES_H */
