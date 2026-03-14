/**
 * @file app.h
 * @brief PinguinOS App-Verwaltung – Laden und Ausführen von Anwendungen.
 *
 * Unterstützt:
 *   - Integrierte Mini-Apps (Notepad, Rechner, Terminal)
 *   - PE/EXE-Dateien über den PE-Loader
 *   - Fenster-Verwaltung für laufende Apps
 */
#ifndef _OS_APP_H
#define _OS_APP_H

#include "../../kernal/include/types.h"
#include "../../kernal/include/pe.h"

/* ── Maximale Anzahl gleichzeitig laufender Apps ─────────────────── */
#define APP_MAX_RUNNING   8
#define APP_NAME_MAX     32
#define APP_MAX_WINDOWS   8

/* ── Fenster-Zustand ─────────────────────────────────────────────── */
typedef struct {
    int       x, y, w, h;       /* Position und Größe               */
    char      title[APP_NAME_MAX];
    bool      visible;
    bool      focused;
    bool      dragging;
    int       drag_ox, drag_oy;  /* Offset beim Drag                */
    /* Inhalt: einfacher Text-Buffer für Placeholder-Apps */
    char      content[512];
    int       content_lines;
    uint32_t  bg_color;
    uint32_t  title_color;
} app_window_t;

/* ── App-Typ ─────────────────────────────────────────────────────── */
typedef enum {
    APP_TYPE_BUILTIN,    /* Eingebaute Mini-App               */
    APP_TYPE_PE_EXE,     /* PE32-Executable                   */
} app_type_t;

/* ── Laufende App ────────────────────────────────────────────────── */
typedef struct {
    char             name[APP_NAME_MAX];
    app_type_t       type;
    bool             running;
    app_window_t     window;
    pe_loaded_image_t pe_image;  /* Nur für PE-Apps               */
    void           (*builtin_fn)(app_window_t *win);  /* Für Builtins */
} app_instance_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** App-System initialisieren. */
void app_init(void);

/** Eingebaute Mini-App starten. */
int app_launch_builtin(const char *name, const char *title,
                       void (*fn)(app_window_t *win));

/** PE/EXE-Datei laden und starten. */
int app_launch_exe(const char *path);

/** App schließen (nach Index). */
void app_close(int index);

/** Alle App-Fenster zeichnen. */
void app_draw_windows(void);

/** Maus-Event an App-Fenster weiterleiten.
 *  @return true wenn ein Fenster das Event verarbeitet hat. */
bool app_handle_mouse(int mx, int my, uint8_t buttons,
                      uint8_t clicked, uint8_t released);

/** Tastatur-Event an fokussiertes Fenster weiterleiten. */
void app_handle_key(uint8_t scancode, bool released);

/** Anzahl der laufenden Apps. */
int app_running_count(void);

/** Zugriff auf App-Instanz (für Taskleiste etc.). */
app_instance_t *app_get_instance(int index);

#endif /* _OS_APP_H */
