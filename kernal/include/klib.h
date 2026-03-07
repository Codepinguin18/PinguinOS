/**
 * @file klib.h
 * @brief Minimal freestanding string and memory utilities for PinguinOS.
 *
 * Because we compile with -ffreestanding and do not link against any libc,
 * we provide our own small implementations of the most common libc helpers.
 */
#ifndef _KLIB_H
#define _KLIB_H

#include "types.h"

/* ── Memory ──────────────────────────────────────────────────────── */

/** Fill @p n bytes starting at @p s with value @p c. */
void *memset(void *s, int c, size_t n);

/** Copy @p n bytes from @p src to @p dst (regions must not overlap). */
void *memcpy(void *dst, const void *src, size_t n);

/** Copy @p n bytes, handling overlapping regions correctly. */
void *memmove(void *dst, const void *src, size_t n);

/** Compare @p n bytes of @p a and @p b; returns <0, 0, or >0. */
int   memcmp(const void *a, const void *b, size_t n);

/* ── Strings ─────────────────────────────────────────────────────── */

/** Return length of null-terminated string @p s. */
size_t strlen(const char *s);

/** Copy string @p src into @p dst; dst must have room for src + NUL. */
char  *strcpy(char *dst, const char *src);

/** Copy at most @p n bytes of @p src into @p dst; always NUL-terminates. */
char  *strncpy(char *dst, const char *src, size_t n);

/** Append @p src to @p dst. */
char  *strcat(char *dst, const char *src);

/** Compare two strings; returns <0, 0, or >0. */
int    strcmp(const char *a, const char *b);

/** Compare at most @p n chars. */
int    strncmp(const char *a, const char *b, size_t n);

/** Find first occurrence of @p c in @p s, or NULL. */
char  *strchr(const char *s, int c);

/** Reverse a string in place; returns @p s. */
char  *strrev(char *s);

/* ── Number formatting ───────────────────────────────────────────── */

/**
 * @brief Convert unsigned integer @p val to a string in the given base.
 * @param buf   Output buffer (must be large enough).
 * @param val   Value to convert.
 * @param base  Radix (2–36).
 * @param upper Use uppercase hex digits.
 * @return Number of characters written (not including NUL).
 */
int uitoa(char *buf, uint32_t val, int base, bool upper);

/** Convert signed integer to decimal string. */
int itoa(char *buf, int32_t val);

/**
 * @brief Minimal vsnprintf implementation.
 *
 * Supported specifiers: %c, %s, %d, %i, %u, %x, %X, %p, %%.
 * Supports width and zero-padding (e.g. %08x).
 *
 * @return Number of characters that would have been written
 *         (excluding the null terminator), similar to C99 snprintf.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap);

/** Wrapper around vsnprintf. */
int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* _KLIB_H */
