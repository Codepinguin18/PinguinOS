/**
 * @file ui.c
 * @brief PinguinOS UI-Zeichenengine – doppelgepufferter BGA-Framebuffer.
 *
 * Alle Zeichenoperationen schreiben in einen RAM-Backbuffer (g_back).
 * ui_flip() kopiert g_back → echten FB → kein Flackern.
 *
 * Font-Rendering FIX: Bit 0 = linkster Pixel (1u << col).
 */

#include "../include/ui.h"
#include "../include/font8x8.h"
#include "../../kernal/include/bga.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/serial.h"

/* ── Framebuffer & Backbuffer ─────────────────────────────────────── */
static uint32_t *g_fb   = (uint32_t *)0xE0000000u;
static uint32_t  g_back[SCREEN_W * SCREEN_H];   /* 3 MB RAM-Puffer  */
static int       g_width  = SCREEN_W;
static int       g_height = SCREEN_H;

static inline void fb_put(int x, int y, uint32_t col)
{
    if ((unsigned)x < (unsigned)g_width && (unsigned)y < (unsigned)g_height)
        g_back[y * g_width + x] = col;
}

/* ── Initialisierung ──────────────────────────────────────────────── */
void ui_init(void)
{
    bga_set_mode(SCREEN_W, SCREEN_H, 32);
    void *fb = bga_get_framebuffer();
    if (fb) {
        g_fb = (uint32_t *)fb;
        serial_printf("[UI] FB @ 0x%08x  backbuf @ 0x%08x\n",
                      (uint32_t)fb, (uint32_t)g_back);
    } else {
        serial_puts("[UI] FEHLER: kein FB – verwende 0xE0000000\n");
        g_fb = (uint32_t *)0xE0000000u;
    }
    g_width  = SCREEN_W;
    g_height = SCREEN_H;
    memset(g_back, 0, sizeof(g_back));
    ui_flip();
    serial_puts("[UI] Doppelpuffer bereit\n");
}

/* ── Flip: Backbuffer → Framebuffer (einmaliger Schreibzug) ──────── */
void ui_flip(void)
{
    uint32_t total = (uint32_t)(g_width * g_height);
    for (uint32_t i = 0; i < total; i++)
        g_fb[i] = g_back[i];
}

/* ── Primitiven (schreiben alle in g_back) ────────────────────────── */
void ui_put_pixel(int x, int y, uint32_t colour)
{
    fb_put(x, y, colour);
}

void ui_clear(uint32_t colour)
{
    int total = g_width * g_height;
    for (int i = 0; i < total; i++)
        g_back[i] = colour;
}

void ui_fill_rect(int x, int y, int w, int h, uint32_t colour)
{
    int x1 = x < 0 ? 0 : x;
    int y1 = y < 0 ? 0 : y;
    int x2 = x + w; if (x2 > g_width)  x2 = g_width;
    int y2 = y + h; if (y2 > g_height) y2 = g_height;
    for (int row = y1; row < y2; row++) {
        uint32_t *line = g_back + row * g_width;
        for (int col = x1; col < x2; col++)
            line[col] = colour;
    }
}

void ui_draw_rect(int x, int y, int w, int h, uint32_t colour)
{
    ui_hline(x,     y,     w, colour);
    ui_hline(x,     y+h-1, w, colour);
    ui_vline(x,     y,     h, colour);
    ui_vline(x+w-1, y,     h, colour);
}

void ui_hline(int x, int y, int len, uint32_t colour)
{
    if (y < 0 || y >= g_height) return;
    int x1 = x < 0 ? 0 : x;
    int x2 = x + len; if (x2 > g_width) x2 = g_width;
    uint32_t *line = g_back + y * g_width;
    for (int col = x1; col < x2; col++) line[col] = colour;
}

void ui_vline(int x, int y, int len, uint32_t colour)
{
    if (x < 0 || x >= g_width) return;
    int y1 = y < 0 ? 0 : y;
    int y2 = y + len; if (y2 > g_height) y2 = g_height;
    for (int row = y1; row < y2; row++)
        g_back[row * g_width + x] = colour;
}

void ui_draw_shadow_box(int x, int y, int w, int h)
{
    ui_fill_rect(x+4, y+4, w,   h,   0x02020A);
    ui_fill_rect(x+2, y+2, w+1, h+1, 0x05050E);
    ui_fill_rect(x, y, w, h, COL_PANEL);
    ui_hline(x, y,     w, COL_ACCENT_DIM);
    ui_hline(x, y+h-1, w, COL_DARK_GRAY);
    ui_vline(x,     y, h, COL_DARK_GRAY);
    ui_vline(x+w-1, y, h, COL_DARK_GRAY);
}

void ui_draw_separator(int x, int y, int w)
{
    ui_hline(x, y,   w, COL_SEPARATOR);
    ui_hline(x, y+1, w, 0x252535);
}

/* ── Schrift (FIX: bit0 = linkster Pixel) ────────────────────────── */
int ui_draw_char(int x, int y, char c, uint32_t colour)
{
    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = '?';
    const uint8_t *bitmap = font8x8[(uint8_t)c - 32];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = bitmap[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1u << col))   /* bit0 = links */
                fb_put(x + col, y + row, colour);
        }
    }
    return x + FONT_W + 1;
}

int ui_draw_text(int x, int y, const char *s, uint32_t colour)
{
    while (*s) {
        x = ui_draw_char(x, y, *s++, colour);
        if (x >= g_width) break;
    }
    return x;
}

void ui_draw_text_centered(int rx, int ry, int rw, int rh,
                            const char *s, uint32_t colour)
{
    int len = (int)strlen(s);
    int tw  = len * (FONT_W + 1);
    int tx  = rx + (rw - tw) / 2;
    int ty  = ry + (rh - FONT_H) / 2;
    ui_draw_text(tx, ty, s, colour);
}

/* ── Schaltfläche ─────────────────────────────────────────────────── */
void ui_draw_button(int x, int y, int w, int h,
                    const char *label, bool hover)
{
    uint32_t bg = hover ? COL_BTN_HOVER : COL_BTN_BG;
    ui_fill_rect(x, y, w, h, bg);
    ui_draw_rect(x, y, w, h, hover ? COL_WHITE : COL_ACCENT_DIM);
    int len = (int)strlen(label);
    int tw  = len * (FONT_W + 1);
    int tx  = x + (w - tw) / 2;
    int ty  = y + (h - FONT_H) / 2;
    ui_draw_text(tx, ty, label, COL_BTN_TEXT);
}

/* ── Eingabefeld ──────────────────────────────────────────────────── */
void ui_draw_input(int x, int y, int w, int h,
                   const char *text, bool focused, bool secret)
{
    uint32_t border = focused ? COL_INPUT_FOCUS : COL_INPUT_IDLE;
    ui_fill_rect(x, y, w, h, COL_INPUT_BG);
    ui_draw_rect(x, y, w, h, border);
    if (focused) ui_draw_rect(x+1, y+1, w-2, h-2, 0x00887744);

    int tx = x + 8;
    int ty = y + (h - FONT_H) / 2;
    if (text && *text) {
        if (secret) {
            int count = (int)strlen(text);
            for (int i = 0; i < count && tx + 8 < x + w - 4; i++) {
                ui_fill_rect(tx+1, ty+1, 5, 5, COL_ACCENT);
                tx += 9;
            }
        } else {
            const char *p   = text;
            int avail       = w - 16;
            int char_w      = FONT_W + 1;
            int max_chars   = avail / char_w;
            int len         = (int)strlen(p);
            if (len > max_chars) p += (len - max_chars);
            ui_draw_text(tx, ty, p, COL_WHITE);
        }
    }

    if (focused) {
        int clen = text ? (int)strlen(text) : 0;
        int cx;
        if (secret) {
            cx = tx + clen * 9;
        } else {
            int avail     = w - 16;
            int char_w    = FONT_W + 1;
            int max_chars = avail / char_w;
            int shown     = clen < max_chars ? clen : max_chars;
            cx = x + 8 + shown * char_w;
        }
        if (cx < x + w - 4)
            ui_vline(cx, ty, FONT_H, COL_ACCENT);
    }
}

/* ── Icon (10×10 → ICON_DRAW_SIZE upscale) ──────────────────────── */
void ui_draw_icon(int x, int y, const uint32_t *pixels_10x10)
{
    int scale = ICON_DRAW_SIZE / ICON_RAW_SIZE;
    int off   = (ICON_DRAW_SIZE - ICON_RAW_SIZE * scale) / 2;
    for (int row = 0; row < ICON_RAW_SIZE; row++) {
        for (int col = 0; col < ICON_RAW_SIZE; col++) {
            uint32_t c = pixels_10x10[row * ICON_RAW_SIZE + col];
            if (c == 0x00000000) c = COL_ICON_BG;
            ui_fill_rect(x + off + col * scale,
                         y + off + row * scale,
                         scale, scale, c);
        }
    }
}

/* ── Icon-Kachel ─────────────────────────────────────────────────── */
void ui_draw_icon_tile(int x, int y, const uint32_t *pixels_10x10,
                        const char *label, bool selected)
{
    int tile_w = 64, tile_h = 80;
    uint32_t bg = selected ? COL_HOVER : COL_BG;
    ui_fill_rect(x, y, tile_w, tile_h, bg);
    if (selected)
        ui_draw_rect(x, y, tile_w, tile_h, COL_ACCENT_DIM);

    int ix = x + (tile_w - ICON_DRAW_SIZE) / 2;
    int iy = y + 4;
    ui_fill_rect(ix-2, iy-2, ICON_DRAW_SIZE+4, ICON_DRAW_SIZE+4, COL_ICON_BG);
    ui_draw_rect(ix-2, iy-2, ICON_DRAW_SIZE+4, ICON_DRAW_SIZE+4, COL_DARK_GRAY);
    ui_draw_icon(ix, iy, pixels_10x10);

    int llen = (int)strlen(label);
    int lw   = llen * (FONT_W + 1);
    int lx   = x + (tile_w - lw) / 2;
    if (lx < x) lx = x;
    ui_draw_text(lx, iy + ICON_DRAW_SIZE + 6, label,
                 selected ? COL_ACCENT : COL_WHITE);
}