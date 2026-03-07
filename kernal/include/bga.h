/**
 * @file bga.h
 * @brief Bochs Graphics Adapter (BGA/VBE) driver API for PinguinOS.
 *
 * Feature #48 – BGA Graphics (linear framebuffer via Bochs VBE extension).
 * Compatible with QEMU's stdvga / bochs-display devices.
 */
#ifndef _BGA_H
#define _BGA_H

#include "types.h"

/* ── Common resolutions ──────────────────────────────────────────── */
#define BGA_RES_640x480   640,  480
#define BGA_RES_800x600   800,  600
#define BGA_RES_1024x768  1024, 768
#define BGA_RES_1280x720  1280, 720

/* ── Public API ──────────────────────────────────────────────────── */

/**
 * @brief Detect whether BGA hardware is present.
 * @return true if BGA ID register reads a valid value.
 */
bool bga_detect(void);

/**
 * @brief Set a video mode.
 * @param width   Horizontal resolution in pixels.
 * @param height  Vertical resolution in pixels.
 * @param bpp     Colour depth (8, 16, 24, or 32).
 * @return true on success, false if BGA not available or mode invalid.
 */
bool bga_set_mode(uint16_t width, uint16_t height, uint16_t bpp);

/** Disable BGA (fall back to VGA text mode). */
void bga_disable(void);

/** @return Pointer to the linear framebuffer, or NULL if not active. */
void *bga_get_framebuffer(void);

/** @return Current framebuffer pitch (bytes per scanline). */
uint32_t bga_get_pitch(void);

/** @return Current width in pixels. */
uint16_t bga_get_width(void);

/** @return Current height in pixels. */
uint16_t bga_get_height(void);

/** @return Bits per pixel of the current mode. */
uint16_t bga_get_bpp(void);

/**
 * @brief Write a 32-bit XRGB pixel at (x, y).
 * No bounds checking – caller must ensure x < width, y < height.
 */
void bga_put_pixel(uint16_t x, uint16_t y, uint32_t colour);

/** Fill the entire framebuffer with a colour. */
void bga_clear(uint32_t colour);

/**
 * @brief Fill a rectangle.
 * @param x, y   Top-left corner.
 * @param w, h   Width and height.
 * @param colour 32-bit XRGB colour.
 */
void bga_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                   uint32_t colour);

#endif /* _BGA_H */