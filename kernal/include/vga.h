/**
 * @file vga.h
 * @brief VGA-Textmodus und grundlegende Framebuffer-Ausgabe für PinguinOS.
 *
 * Unterstützt:
 *  – 80×25 VGA-Textmodus (Memory-mapped bei 0xB8000).
 *  – Farb-Attributbyte-Kodierung (Vordergrund- + Hintergrundfarbe).
 *  – Cursor-Positionierung über VGA-CRTC-Ports.
 *  – Formatierte Ausgabe im printf-Stil (vga_printf).
 *  – Optionaler linearer 32-bpp Framebuffer, falls GRUB einen übergibt.
 */
#ifndef _VGA_H
#define _VGA_H

#include "types.h"
#include "multiboot.h"

/* ── Textmodus-Dimensionen ───────────────────────────────────────── */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  ((volatile uint16_t *)0xB8000)

/* ── VGA-CRTC-Portadressen ───────────────────────────────────────── */
#define VGA_CRTC_ADDR  0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_CURSOR_HI  0x0E
#define VGA_CURSOR_LO  0x0F

/* ── Farbkonstanten (Standard 4-Bit VGA-Palette) ────────────────── */
typedef enum vga_color {
    VGA_BLACK         = 0,
    VGA_BLUE          = 1,
    VGA_GREEN         = 2,
    VGA_CYAN          = 3,
    VGA_RED           = 4,
    VGA_MAGENTA       = 5,
    VGA_BROWN         = 6,
    VGA_LIGHT_GREY    = 7,
    VGA_DARK_GREY     = 8,
    VGA_LIGHT_BLUE    = 9,
    VGA_LIGHT_GREEN   = 10,
    VGA_LIGHT_CYAN    = 11,
    VGA_LIGHT_RED     = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW        = 14,
    VGA_WHITE         = 15,
} vga_color_t;

/** Packt eine Vordergrund- und Hintergrundfarbe in ein VGA-Attributbyte. */
static INLINE uint8_t vga_make_attr(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)((bg << 4) | (fg & 0x0F));
}

/** Kombiniert ein Zeichen und ein Attributbyte zu einer 16-Bit VGA-Zelle. */
static INLINE uint16_t vga_make_entry(char c, uint8_t attr) {
    return (uint16_t)((uint16_t)attr << 8) | (uint8_t)c;
}

/* ── Öffentliche API – Textmodus ─────────────────────────────────── */

/**
 * @brief Initialisiert den VGA-Text-Treiber.
 *
 * Leert den Bildschirm und positioniert den Cursor bei (0,0).
 * Sollte früh in der Kernel-Initialisierung vor jeder Ausgabe aufgerufen werden.
 */
void vga_init(void);

/** Leert den gesamten Bildschirm mit der aktuellen Hintergrundfarbe. */
void vga_clear(void);

/** Setzt die aktive Vordergrundfarbe. */
void vga_set_fg(vga_color_t color);

/** Setzt die aktive Hintergrundfarbe. */
void vga_set_bg(vga_color_t color);

/** Setzt Vordergrund- und Hintergrundfarbe gleichzeitig. */
void vga_set_color(vga_color_t fg, vga_color_t bg);

/**
 * @brief Schreibt ein einzelnes Zeichen an die aktuelle Cursorposition.
 *
 * Behandelt \\n (Zeilenumbruch), \\r, \\t (4-Leerzeichen Tab) und \\b (Rücktaste).
 * Scrollt den Bildschirm eine Zeile nach oben, wenn der Cursor den unteren Rand erreicht.
 */
void vga_putchar(char c);

/** Schreibt einen null-terminierten String. */
void vga_puts(const char *str);

/**
 * @brief Formatierte Ausgabe auf VGA (Untermenge von printf).
 *
 * Unterstützte Format-Spezifizierer: %c, %s, %d, %i, %u, %x, %X, %p, %%.
 * Breite und Null-Padding (z.B. %08x) werden unterstützt.
 */
void vga_printf(const char *fmt, ...);

/** Bewegt den Hardware-Cursor nach (Spalte, Zeile). */
void vga_set_cursor(uint8_t col, uint8_t row);

/** Gibt die aktuelle Cursor-Spalte zurück (0-basiert). */
uint8_t vga_get_col(void);

/** Gibt die aktuelle Cursor-Zeile zurück (0-basiert). */
uint8_t vga_get_row(void);

/* ── Öffentliche API – Linearer Framebuffer ─────────────────────── */

/**
 * @brief Initialisiert einen linearen (VESA/GOP) Framebuffer, falls verfügbar.
 * @param mbi  Multiboot-Info; Framebuffer-Felder werden daraus gelesen.
 * @return true, falls ein Framebuffer gefunden und initialisiert wurde.
 */
bool fb_init(multiboot_info_t *mbi);

/** Zeichnet ein Pixel bei (x,y) mit dem 32-bpp Farbwert @p color. */
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);

/** Leert den gesamten Framebuffer mit @p color. */
void fb_clear(uint32_t color);

/** Zeichnet ein gefülltes Rechteck. */
void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/** Zeichnet ein ASCII-Zeichen an der Framebuffer-Pixelposition (x,y). */
void fb_put_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);

/** Zeichnet einen null-terminierten String beginnend bei Pixel (x,y). */
void fb_puts(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);

#endif /* _VGA_H */
