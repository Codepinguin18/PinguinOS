/**
 * @file klib.c
 * @brief Eigenständige Datei für String-, Speicher- und Formatierungs-Hilfsfunktionen.
 */

#include "../include/klib.h"
#include "../include/types.h"

/* ── Speicher (Memory) ───────────────────────────────────────────── */

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
    while (n--) *p++ = (uint8_t)c;
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t       *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    uint8_t       *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    while (n--) {
        if (*pa != *pb) return (int)*pa - (int)*pb;
        pa++; pb++;
    }
    return 0;
}

/* ── Strings ─────────────────────────────────────────────────────── */

size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++) n++;
    return n;
}

char *strcpy(char *dst, const char *src)
{
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    char *d = dst;
    while (n && (*d++ = *src++)) n--;
    if (n) *d = '\0';
    return dst;
}

char *strcat(char *dst, const char *src)
{
    char *d = dst;
    while (*d) d++;
    while ((*d++ = *src++));
    return dst;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n)
{
    while (n && *a && *a == *b) { a++; b++; n--; }
    if (!n) return 0;
    return (unsigned char)*a - (unsigned char)*b;
}

char *strchr(const char *s, int c)
{
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return NULL;
}

char *strrev(char *s)
{
    size_t len = strlen(s);
    for (size_t i = 0; i < len / 2; i++) {
        char tmp    = s[i];
        s[i]        = s[len - 1 - i];
        s[len-1-i]  = tmp;
    }
    return s;
}

/* ── Zahlenformatierung (Number formatting) ───────────────────────── */

int uitoa(char *buf, uint32_t val, int base, bool upper)
{
    static const char digits_lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char digits_upper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = upper ? digits_upper : digits_lower;

    if (base < 2 || base > 36) { buf[0] = '\0'; return 0; }

    int len = 0;
    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val) {
            buf[len++] = digits[val % (uint32_t)base];
            val /= (uint32_t)base;
        }
        /* Ziffern liegen in umgekehrter Reihenfolge vor */
        for (int i = 0, j = len - 1; i < j; i++, j--) {
            char tmp = buf[i]; buf[i] = buf[j]; buf[j] = tmp;
        }
    }
    buf[len] = '\0';
    return len;
}

int itoa(char *buf, int32_t val)
{
    int len = 0;
    if (val < 0) {
        buf[len++] = '-';
        val = -val;
    }
    len += uitoa(buf + len, (uint32_t)val, 10, false);
    return len;
}

/* ── vsnprintf ───────────────────────────────────────────────────── */

/*
 * Minimales vsnprintf mit Unterstützung für:
 *   %c  Einzelnes Zeichen (Character)
 *   %s  Zeichenkette (String)
 *   %d  Vorzeichenbehaftete Dezimalzahl
 *   %i  Vorzeichenbehaftete Dezimalzahl (Alias)
 *   %u  Vorzeichenlose Dezimalzahl
 *   %x  Hexadezimal (Kleinschreibung)
 *   %X  Hexadezimal (Großschreibung)
 *   %p  Pointer (0x Präfix, Hex Kleinschreibung)
 *   %%  Literales Prozentzeichen
 *
 * Breite (Width) und Null-Padding werden unterstützt (z. B. %08x, %5d).
 */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap)
{
    size_t pos = 0;
    char tmp[32];

#define EMIT(c)  do { if (pos + 1 < size) buf[pos++] = (c); else pos++; } while(0)

    while (*fmt) {
        if (*fmt != '%') { EMIT(*fmt++); continue; }
        fmt++;  /* Überspringe '%' */

        /* Optionales Null-Pad-Flag parsen */
        char pad_char = ' ';
        if (*fmt == '0') { pad_char = '0'; fmt++; }

        /* Optionale Breite parsen */
        int width = 0;
        while (*fmt >= '0' && *fmt <= '9')
            width = width * 10 + (*fmt++ - '0');

        /* Konvertierungsspezifizierer parsen */
        switch (*fmt) {
        case 'c': {
            char c = (char)__builtin_va_arg(ap, int);
            EMIT(c);
            break;
        }
        case 's': {
            const char *s = __builtin_va_arg(ap, const char *);
            if (!s) s = "(null)";
            int slen = (int)strlen(s);
            for (int i = slen; i < width; i++) EMIT(' ');
            while (*s) EMIT(*s++);
            break;
        }
        case 'd':
        case 'i': {
            int32_t v = __builtin_va_arg(ap, int32_t);
            int len = itoa(tmp, v);
            for (int i = len; i < width; i++) EMIT(pad_char);
            for (int i = 0; i < len; i++) EMIT(tmp[i]);
            break;
        }
        case 'u': {
            uint32_t v = __builtin_va_arg(ap, uint32_t);
            int len = uitoa(tmp, v, 10, false);
            for (int i = len; i < width; i++) EMIT(pad_char);
            for (int i = 0; i < len; i++) EMIT(tmp[i]);
            break;
        }
        case 'x':
        case 'X': {
            uint32_t v = __builtin_va_arg(ap, uint32_t);
            bool upper = (*fmt == 'X');
            int len = uitoa(tmp, v, 16, upper);
            for (int i = len; i < width; i++) EMIT(pad_char);
            for (int i = 0; i < len; i++) EMIT(tmp[i]);
            break;
        }
        case 'p': {
            uintptr_t v = (uintptr_t)__builtin_va_arg(ap, void *);
            EMIT('0'); EMIT('x');
            int len = uitoa(tmp, (uint32_t)v, 16, false);
            for (int i = len; i < 8; i++) EMIT('0');
            for (int i = 0; i < len; i++) EMIT(tmp[i]);
            break;
        }
        case '%':
            EMIT('%');
            break;
        default:
            EMIT('%');
            EMIT(*fmt);
            break;
        }
        fmt++;
    }

    /* Null-Terminierung innerhalb des Puffers */
    if (size > 0)
        buf[pos < size ? pos : size - 1] = '\0';

    return (int)pos;

#undef EMIT
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    int r = vsnprintf(buf, size, fmt, ap);
    __builtin_va_end(ap);
    return r;
}
