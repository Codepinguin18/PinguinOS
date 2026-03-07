/**
 * @file klib.h
 * @brief Minimale freistehende String- und Speicherhilfen für PinguinOS.
 *
 * Da wir mit -ffreestanding kompilieren und nicht gegen eine libc linken,
 * stellen wir unsere eigenen kleinen Implementierungen der üblichsten Libc-Helfer bereit.
 */
#ifndef _KLIB_H
#define _KLIB_H

#include "types.h"

/* ── Speicher (Memory) ───────────────────────────────────────────── */

/** Füllt @p n Bytes ab @p s mit dem Wert @p c. */
void *memset(void *s, int c, size_t n);

/** Kopiert @p n Bytes von @p src nach @p dst (Bereiche dürfen sich nicht überlappen). */
void *memcpy(void *dst, const void *src, size_t n);

/** Kopiert @p n Bytes, wobei sich überlappende Bereiche korrekt behandelt werden. */
void *memmove(void *dst, const void *src, size_t n);

/** Vergleicht die ersten @p n Bytes von @p a und @p b; liefert <0, 0, oder >0. */
int   memcmp(const void *a, const void *b, size_t n);

/* ── Strings ─────────────────────────────────────────────────────── */

/** Liefert die Länge des null-terminierten Strings @p s. */
size_t strlen(const char *s);

/** Kopiert String @p src nach @p dst; dst muss genug Platz für src + NUL bieten. */
char  *strcpy(char *dst, const char *src);

/** Kopiert maximal @p n Bytes von @p src nach @p dst; terminiert immer mit NUL. */
char  *strncpy(char *dst, const char *src, size_t n);

/** Hängt @p src an @p dst an. */
char  *strcat(char *dst, const char *src);

/** Vergleicht zwei Strings; liefert <0, 0, oder >0. */
int    strcmp(const char *a, const char *b);

/** Vergleicht maximal @p n Zeichen. */
int    strncmp(const char *a, const char *b, size_t n);

/** Findet das erste Vorkommen von @p c in @p s, oder NULL. */
char  *strchr(const char *s, int c);

/** Kehrt einen String an Ort und Stelle um; liefert @p s. */
char  *strrev(char *s);

/* ── Zahlenformatierung ──────────────────────────────────────────── */

/**
 * @brief Wandelt eine vorzeichenlose Ganzzahl @p val in einen String zur gegebenen Basis um.
 * @param buf   Ausgabepuffer (muss groß genug sein).
 * @param val   Zu konvertierender Wert.
 * @param base  Zahlenbasis (2–36).
 * @param upper Verwende Großbuchstaben für Hex-Ziffern.
 * @return Anzahl der geschriebenen Zeichen (ohne NUL).
 */
int uitoa(char *buf, uint32_t val, int base, bool upper);

/** Wandelt eine vorzeichenbehaftete Ganzzahl in einen Dezimal-String um. */
int itoa(char *buf, int32_t val);

/**
 * @brief Minimale vsnprintf-Implementierung.
 *
 * Unterstützte Spezifizierer: %c, %s, %d, %i, %u, %x, %X, %p, %%.
 * Unterstützt Breite und Null-Padding (z.B. %08x).
 *
 * @return Anzahl der Zeichen, die geschrieben worden wären (ohne Null-Terminator).
 */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap);

/** Wrapper um vsnprintf. */
int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* _KLIB_H */
