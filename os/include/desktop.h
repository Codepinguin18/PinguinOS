/**
 * @file desktop.h
 * @brief PinguinOS Desktop-Umgebung.
 */
#ifndef _OS_DESKTOP_H
#define _OS_DESKTOP_H

#include "../../kernal/include/types.h"
#include "login.h"

#define DESKTOP_MAX_ICONS   16

typedef struct {
    int           x, y;          /* Position auf dem Desktop          */
    const char   *label;         /* Bezeichnung                       */
    const uint32_t *icon_pixels; /* 10×10 XRGB-Pixelarray             */
    void        (*on_click)(void); /* Klick-Handler                   */
    bool          selected;
} desktop_icon_t;

typedef enum {
    DESKTOP_RUNNING,
    DESKTOP_LOGOUT,
    DESKTOP_SHUTDOWN
} desktop_state_t;

/**
 * @brief Desktop starten.
 * @param user  Eingeloggter Benutzer.
 * @return Zustand nach Beenden (LOGOUT oder SHUTDOWN).
 */
desktop_state_t desktop_run(const os_user_t *user);

#endif /* _OS_DESKTOP_H */