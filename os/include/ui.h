/**
 * @file ui.h
 * @brief PinguinOS Desktop UI – Zeichenprimitiven und Farbpalette.
 *
 * Alle Farben als 32-Bit XRGB (0x00RRGGBB) passend zum
 * Pinguin-Browser-Design (dunkel + Teal-Akzent).
 */
#ifndef _OS_UI_H
#define _OS_UI_H

#include "../../kernal/include/types.h"
#include "../../kernal/include/bga.h"
#include "../../kernal/include/klib.h"

/* ── Farbpalette (Pinguin-Browser-Style) ──────────────────────── */
#define COL_BG          0x07070F   /* Hintergrund (fast schwarz-blau)   */
#define COL_PANEL       0x0C0C1A   /* Panel / Taskleiste                */
#define COL_PANEL2      0x10101E   /* Helleres Panel                    */
#define COL_ACCENT      0x00FFF0   /* Teal-Akzentfarbe                  */
#define COL_ACCENT_DIM  0x009988   /* Gedämpftes Teal                   */
#define COL_WHITE       0xFFFFFF   /* Weißer Text                       */
#define COL_GRAY        0x888899   /* Grauer Text                       */
#define COL_DARK_GRAY   0x333344   /* Dunkler Rand                      */
#define COL_HOVER       0x1A1A30   /* Hover-Hintergrund                 */
#define COL_INPUT_BG    0x0F0F22   /* Eingabefeld-Hintergrund           */
#define COL_INPUT_FOCUS 0x00FFF0   /* Eingabefeld-Rand aktiv            */
#define COL_INPUT_IDLE  0x333355   /* Eingabefeld-Rand inaktiv          */
#define COL_BTN_BG      0x00FFF0   /* Schaltflächen-Hintergrund         */
#define COL_BTN_TEXT    0x07070F   /* Schaltflächen-Text                */
#define COL_BTN_HOVER   0x00CCCC   /* Schaltflächen-Hover               */
#define COL_SHADOW      0x02020A   /* Schatten                          */
#define COL_SEPARATOR   0x1E1E35   /* Trennlinie                        */
#define COL_ERROR       0xFF4444   /* Fehler (rot)                      */
#define COL_SUCCESS     0x44FF88   /* Erfolg (grün)                     */
#define COL_ICON_BG     0x131328   /* Icon-Hintergrund auf dem Desktop  */

/* ── Bildschirmauflösung ──────────────────────────────────────── */
#define SCREEN_W  1280
#define SCREEN_H   720

/* ── Taskleiste ───────────────────────────────────────────────── */
#define TASKBAR_H    40
#define TASKBAR_Y    (SCREEN_H - TASKBAR_H)

/* ── Icons ────────────────────────────────────────────────────── */
#define ICON_RAW_SIZE   10          /* 10×10 Pixel Quelldatei         */
#define ICON_DRAW_SIZE  32          /* Gezeichnete Größe (upscaled)   */

/* ── Bitmap-Font ──────────────────────────────────────────────── */
#define FONT_W   6    /* Pixel pro Zeichen (mit 1px Abstand)       */
#define FONT_H   8    /* Pixel pro Zeile                           */

/* ── Öffentliche API ──────────────────────────────────────────── */

/** UI-Schicht initialisieren (BGA-Modus setzen). */
void ui_init(void);

/** Gesamten Bildschirm mit Farbe füllen. */
void ui_clear(uint32_t colour);

/** Rechteck zeichnen (gefüllt). */
void ui_fill_rect(int x, int y, int w, int h, uint32_t colour);

/** Rechteck-Rahmen zeichnen (nicht gefüllt). */
void ui_draw_rect(int x, int y, int w, int h, uint32_t colour);

/** Horizontale Linie. */
void ui_hline(int x, int y, int len, uint32_t colour);

/** Vertikale Linie. */
void ui_vline(int x, int y, int len, uint32_t colour);

/** Einzelnen Pixel setzen (mit Bounds-Check). */
void ui_put_pixel(int x, int y, uint32_t colour);

/**
 * @brief Ein Zeichen zeichnen (8×8 Bitmap-Font).
 * @return x-Position nach dem Zeichen.
 */
int ui_draw_char(int x, int y, char c, uint32_t colour);

/**
 * @brief Zeichenkette zeichnen.
 * @return x-Position nach der Zeichenkette.
 */
int ui_draw_text(int x, int y, const char *s, uint32_t colour);

/**
 * @brief Zeichenkette zentriert in einem Bereich zeichnen.
 */
void ui_draw_text_centered(int rx, int ry, int rw, int rh,
                            const char *s, uint32_t colour);

/**
 * @brief Schaltfläche zeichnen.
 * @param hover  true = Hover-Zustand
 */
void ui_draw_button(int x, int y, int w, int h,
                    const char *label, bool hover);

/**
 * @brief Eingabefeld zeichnen.
 * @param focused true = aktiv (teal Rahmen)
 * @param secret  true = Passwortfeld (● statt Text)
 */
void ui_draw_input(int x, int y, int w, int h,
                   const char *text, bool focused, bool secret);

/**
 * @brief 10×10 Icon-Daten (XRGB, row-major) upscaled auf ICON_DRAW_SIZE zeichnen.
 */
void ui_draw_icon(int x, int y, const uint32_t *pixels_10x10);

/**
 * @brief Icon-Kachel (Icon + Beschriftung darunter) zeichnen.
 */
void ui_draw_icon_tile(int x, int y, const uint32_t *pixels_10x10,
                        const char *label, bool selected);

/** Doppelpuffer auf Framebuffer übertragen (falls doppelte Pufferung aktiv). */
void ui_flip(void);

/** Schatten-Box zeichnen (Rechteck mit subtiler Tiefenwirkung). */
void ui_draw_shadow_box(int x, int y, int w, int h);

/** Trennlinie (horizontal). */
void ui_draw_separator(int x, int y, int w);

#endif /* _OS_UI_H */