/**
 * @file mouse.h
 * @brief PS/2 Mouse driver API for PinguinOS.
 *
 * Feature #32 – PS/2 Mouse
 *   Handles IRQ12, decodes 3-byte packets and exposes a simple event queue.
 *   Supports both relative movement and button state.
 */
#ifndef _MOUSE_H
#define _MOUSE_H

#include "types.h"

/* ── Mouse button mask bits ──────────────────────────────────────── */
#define MOUSE_BTN_LEFT    (1 << 0)
#define MOUSE_BTN_RIGHT   (1 << 1)
#define MOUSE_BTN_MIDDLE  (1 << 2)

/* ── Mouse event ─────────────────────────────────────────────────── */
typedef struct mouse_event {
    int16_t  dx;        /* Relative X movement (-256..+255) */
    int16_t  dy;        /* Relative Y movement (-256..+255, Y-up) */
    uint8_t  buttons;   /* MOUSE_BTN_* bitmask               */
} mouse_event_t;

/* ── Public API ──────────────────────────────────────────────────── */

/** Initialise the PS/2 mouse and register IRQ12 handler. */
void mouse_init(void);

/**
 * @brief Poll for the next mouse event.
 * @param[out] evt  Event structure to fill.
 * @return true if an event was available, false if the queue was empty.
 */
bool mouse_poll(mouse_event_t *evt);

/** @return Current absolute X position (clamped to screen width). */
int32_t mouse_get_x(void);

/** @return Current absolute Y position (clamped to screen height). */
int32_t mouse_get_y(void);

/** @return Current button bitmask. */
uint8_t mouse_get_buttons(void);

/** Set the virtual screen bounds used for absolute position clamping. */
void mouse_set_bounds(int32_t width, int32_t height);

#endif /* _MOUSE_H */