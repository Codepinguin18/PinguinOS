/**
 * @file keyboard.h
 * @brief PS/2 Tastatur-Treiber mit DE/US-Scancode-Tabellen (Feature #31).
 */
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/* ── PS/2-Ports ──────────────────────────────────────────────────── */
#define PS2_DATA_PORT    0x60
#define PS2_STATUS_PORT  0x64
#define PS2_CMD_PORT     0x64

/* Status-Register-Bits */
#define PS2_STATUS_OBF   (1 << 0)   /* Output buffer full   */
#define PS2_STATUS_IBF   (1 << 1)   /* Input  buffer full   */

/* ── Sondertasten (Key-Codes) ────────────────────────────────────── */
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_LCTRL     0x1D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_LALT      0x38
#define KEY_CAPS      0x3A
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F10       0x44
#define KEY_DELETE    0x53
#define KEY_UP        0x48
#define KEY_DOWN      0x50
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D

/* Modifier-Flags */
#define KBD_MOD_SHIFT  (1 << 0)
#define KBD_MOD_CTRL   (1 << 1)
#define KBD_MOD_ALT    (1 << 2)
#define KBD_MOD_CAPS   (1 << 3)

/* ── Tastaturlayout ──────────────────────────────────────────────── */
typedef enum { KBD_LAYOUT_US = 0, KBD_LAYOUT_DE = 1 } kbd_layout_t;

/* ── Tastatur-Event ──────────────────────────────────────────────── */
typedef struct {
    uint8_t scancode;   /* Roh-Scancode                          */
    char    ascii;      /* Übersetzter ASCII-Charakter (0=keiner)*/
    uint8_t modifiers;  /* KBD_MOD_* Flags                      */
    bool    released;   /* true = Taste losgelassen              */
} kbd_event_t;

/* ── Zirkulärer Tastatur-Puffer ──────────────────────────────────── */
#define KBD_BUFFER_SIZE  64

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** Initialisiert den PS/2-Tastatur-Treiber und registriert IRQ1. */
void kbd_init(void);

/** Aktives Layout umschalten. */
void kbd_set_layout(kbd_layout_t layout);

/**
 * @brief Nächstes Tastatur-Event aus dem Puffer lesen.
 * @param event  Wird mit dem nächsten Event gefüllt.
 * @return true wenn ein Event vorhanden war.
 */
bool kbd_poll(kbd_event_t *event);

/**
 * @brief Blockierend einen ASCII-Charakter lesen (nützlich für Shell).
 * @return ASCII-Zeichen (0 für Sondertasten).
 */
char kbd_getchar(void);

/** @return true wenn Zeichen im Puffer. */
bool kbd_has_input(void);

#endif /* _KEYBOARD_H */
