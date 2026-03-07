/**
 * @file vga.h
 * @brief VGA text-mode and basic framebuffer output for PinguinOS.
 *
 * Supports:
 *  – 80×25 VGA text mode (memory-mapped at 0xB8000).
 *  – Colour attribute byte encoding (foreground + background colours).
 *  – Cursor positioning via VGA CRTC ports.
 *  – printf-style formatted output (vga_printf).
 *  – Optional 32-bpp linear framebuffer if GRUB hands one over.
 */
#ifndef _VGA_H
#define _VGA_H

#include "types.h"
#include "multiboot.h"

/* ── Text-mode dimensions ────────────────────────────────────────── */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  ((volatile uint16_t *)0xB8000)

/* ── VGA CRTC port addresses ─────────────────────────────────────── */
#define VGA_CRTC_ADDR  0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_CURSOR_HI  0x0E
#define VGA_CURSOR_LO  0x0F

/* ── Colour constants (standard 4-bit VGA palette) ──────────────── */
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

/** Pack a foreground and background colour into a VGA attribute byte. */
static INLINE uint8_t vga_make_attr(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)((bg << 4) | (fg & 0x0F));
}

/** Combine a character and attribute byte into a 16-bit VGA cell. */
static INLINE uint16_t vga_make_entry(char c, uint8_t attr) {
    return (uint16_t)((uint16_t)attr << 8) | (uint8_t)c;
}

/* ── Public API – text mode ──────────────────────────────────────── */

/**
 * @brief Initialise the VGA text driver.
 *
 * Clears the screen and positions the cursor at (0,0).
 * Should be called early in kernel init before any output.
 */
void vga_init(void);

/** Clear the entire screen with the current background colour. */
void vga_clear(void);

/** Set the active foreground colour. */
void vga_set_fg(vga_color_t color);

/** Set the active background colour. */
void vga_set_bg(vga_color_t color);

/** Set both foreground and background colours at once. */
void vga_set_color(vga_color_t fg, vga_color_t bg);

/**
 * @brief Write a single character at the current cursor position.
 *
 * Handles \\n (newline), \\r, \\t (4-space tab), and \\b (backspace).
 * Scrolls the screen up by one line when the cursor reaches the bottom.
 */
void vga_putchar(char c);

/** Write a null-terminated string. */
void vga_puts(const char *str);

/**
 * @brief Formatted output to VGA (subset of printf).
 *
 * Supported format specifiers: %c, %s, %d, %i, %u, %x, %X, %p, %%.
 * Width and zero-padding (e.g. %08x) are supported.
 */
void vga_printf(const char *fmt, ...);

/** Move the hardware cursor to (col, row). */
void vga_set_cursor(uint8_t col, uint8_t row);

/** Get current cursor column (0-based). */
uint8_t vga_get_col(void);

/** Get current cursor row (0-based). */
uint8_t vga_get_row(void);

/* ── Public API – linear framebuffer ────────────────────────────── */

/**
 * @brief Initialise a linear (VESA/GOP) framebuffer if available.
 * @param mbi  Multiboot info; framebuffer fields are read from it.
 * @return true if a framebuffer was found and initialised.
 */
bool fb_init(multiboot_info_t *mbi);

/** Draw a pixel at (x,y) with 32-bpp colour value @p color. */
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);

/** Fill the entire framebuffer with @p color. */
void fb_clear(uint32_t color);

/** Draw a filled rectangle. */
void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/** Draw an ASCII character at framebuffer pixel position (x,y). */
void fb_put_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);

/** Draw a null-terminated string starting at pixel (x,y). */
void fb_puts(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);

#endif /* _VGA_H */
