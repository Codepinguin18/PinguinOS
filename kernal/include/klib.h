/**
 * @file klib.h
 * @brief Kernel string, memory and formatting utilities – PinguinOS v3.0.
 */
#ifndef _KLIB_H
#define _KLIB_H

#include "types.h"

/* ── Memory ──────────────────────────────────────────────────────── */
void    *memset  (void *s, int c, size_t n);
void    *memcpy  (void *dst, const void *src, size_t n);
void    *memmove (void *dst, const void *src, size_t n);
int      memcmp  (const void *a, const void *b, size_t n);

/* ── Strings ─────────────────────────────────────────────────────── */
size_t   strlen  (const char *s);
char    *strcpy  (char *dst, const char *src);
char    *strncpy (char *dst, const char *src, size_t n);
char    *strcat  (char *dst, const char *src);
char    *strncat (char *dst, const char *src, size_t n);
int      strcmp  (const char *a, const char *b);
int      strncmp (const char *a, const char *b, size_t n);
char    *strchr  (const char *s, int c);
char    *strrchr (const char *s, int c);
char    *strstr  (const char *haystack, const char *needle);
char    *strrev  (char *s);
char    *strdup  (const char *s);   /* heap-allocated; caller kfree()s */

/** Zero-allocation tokeniser.  Modifies *str in place. */
char    *strsplit_next(char **str, char delim);

/* ── Number parsing ──────────────────────────────────────────────── */
uint32_t strtoul (const char *str, char **endptr, int base);
int32_t  strtol  (const char *str, char **endptr, int base);

/* ── Number formatting ───────────────────────────────────────────── */
int uitoa(char *buf, uint32_t val, int base, bool upper);
int itoa (char *buf, int32_t  val);

/* ── Printf-family ───────────────────────────────────────────────── */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap);
int snprintf (char *buf, size_t size, const char *fmt, ...);

/* ── Checksums ───────────────────────────────────────────────────── */
/** One's-complement 16-bit internet checksum (RFC 1071). */
uint16_t klib_checksum16(const void *data, size_t len);
/** Simple byte sum. */
uint8_t  klib_checksum8 (const void *data, size_t len);

/* ── 64-bit math helpers (GCC freestanding requirement) ──────────── */
uint64_t __udivdi3(uint64_t n, uint64_t d);
uint64_t __umoddi3(uint64_t n, uint64_t d);
int64_t  __divdi3 (int64_t n,  int64_t d);
int64_t  __moddi3 (int64_t n,  int64_t d);

#endif /* _KLIB_H */