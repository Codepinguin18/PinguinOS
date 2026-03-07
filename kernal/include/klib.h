/**
 * @file klib.h
 * @brief Minimale eigenständige String- und Speicher-Utilities für PinguinOS.
 *
 * Da wir mit -ffreestanding kompilieren und nicht gegen eine libc linken,
 * stellen wir unsere eigenen kleinen Implementierungen der gängigsten libc-Helfer bereit.
 */
#ifndef _KLIB_H
#define _KLIB_H

#include "types.h"

/* ── Speicher ────────────────────────────────────────────────────── */

/** Füllt @p n Bytes beginnend bei @p s mit dem Wert @p c. */
void *memset(void *s, int c, size_t n);

/** Kopiert @p n Bytes von @p src nach @p dst (Bereiche dürfen sich nicht überlappen). */
void *memcpy(void *dst, const void *src, size_t n);

/** Kopiert @p n Bytes und behandelt überlappende Bereiche korrekt. */
void *memmove(void *dst, const void *src, size_t n);

/** Vergleicht @p n Bytes von @p a und @p b; gibt <0, 0 oder >0 zurück. */
int   memcmp(const void *a, const void *b, size_t n);

/* ── Strings ─────────────────────────────────────────────────────── */

/** Gibt die Länge des null-terminierten Strings @p s zurück. */
size_t strlen(const char *s);

/** Kopiert den String @p src nach @p dst; dst muss Platz für src + NUL haben. */
char  *strcpy(char *dst, const char *src);

/** Kopiert maximal @p n Bytes von @p src nach @p dst; terminiert immer mit NUL. */
char  *strncpy(char *dst, const char *src, size_t n);

/** Hängt @p src an @p dst an. */
char  *strcat(char *dst, const char *src);

/** Vergleicht zwei Strings; gibt <0, 0 oder >0 zurück. */
int    strcmp(const char *a, const char *b);

/** Vergleicht maximal @p n Zeichen. */
int    strncmp(const char *a, const char *b, size_t n);

/** Findet das erste Vorkommen von @p c in @p s, oder NULL. */
char  *strchr(const char *s, int c);

/** Kehrt einen String an Ort und Stelle um; gibt @p s zurück. */
char  *strrev(char *s);

/* ── Zahlenformatierung ──────────────────────────────────────────── */

/**
 * @brief Wandelt eine vorzeichenlose Ganzzahl @p val in einen String zur gegebenen Basis um.
 * @param buf   Ausgabepuffer (muss groß genug sein).
 * @param val   Zu wandelnder Wert.
 * @param base  Zahlensystem-Basis (2–36).
 * @param upper Verwendet Großbuchstaben für Hex-Ziffern.
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
 * @return Anzahl der Zeichen, die geschrieben worden wären
 *         (ohne den Null-Terminator), ähnlich wie C99 snprintf.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap);

/** Wrapper um vsnprintf. */
int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* _KLIB_H */
