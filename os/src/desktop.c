/**
 * @file desktop.c
 * @brief PinguinOS Desktop-Umgebung – Pinguin-Browser-Design.
 *
 * Enthält:
 *   – Wallpaper (dunkel mit Teal-Akzenten)
 *   – Desktop-Icons (Ordner, Terminal, Einstellungen, Browser)
 *   – Taskleiste (unten): Startmenü-Knopf, Uhrzeit, Benutzername
 *   – Startmenü-Overlay mit Abmelden / Herunterfahren
 *   – Maus-Unterstützung für Klicks
 */

#include "../include/desktop.h"
#include "../include/ui.h"
#include "icons.h"
#include "../../kernal/include/keyboard.h"
#include "../../kernal/include/mouse.h"
#include "../../kernal/include/rtc.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/scheduler.h"

/* ══════════════════════════════════════════════════════════════
 *  Desktop-Icon-Layout
 * ══════════════════════════════════════════════════════════════ */
#define ICON_COLS       2
#define ICON_TILE_W     70
#define ICON_TILE_H     85
#define ICON_START_X    24
#define ICON_START_Y    40

/* ── Weiterleitung (Platzhalter-Handler) ─────────────────────── */
static void action_open_terminal(void) { /* TODO: Terminal-App */ }
static void action_open_browser(void)  { /* TODO: Browser-App  */ }
static void action_open_settings(void) { /* TODO: Einstellungen */ }
static void action_open_files(void)    { /* TODO: Dateimanager  */ }

static desktop_icon_t desktop_icons[] = {
    { ICON_START_X,                ICON_START_Y,
      "Dateien",   ICON_FOLDER,   action_open_files,    false },
    { ICON_START_X + ICON_TILE_W,  ICON_START_Y,
      "Terminal",  ICON_TERMINAL, action_open_terminal, false },
    { ICON_START_X,                ICON_START_Y + ICON_TILE_H,
      "Browser",   ICON_BROWSER,  action_open_browser,  false },
    { ICON_START_X + ICON_TILE_W,  ICON_START_Y + ICON_TILE_H,
      "Optionen",  ICON_SETTINGS, action_open_settings, false },
};
#define DESKTOP_ICON_COUNT  4

/* ══════════════════════════════════════════════════════════════
 *  Taskleiste
 * ══════════════════════════════════════════════════════════════ */
#define START_BTN_W   90
#define START_BTN_H   30
#define START_BTN_X   6
#define START_BTN_Y   (TASKBAR_Y + 5)

static void draw_taskbar(const char *username, bool start_open)
{
    /* Hintergrund */
    ui_fill_rect(0, TASKBAR_Y, SCREEN_W, TASKBAR_H, COL_PANEL);
    ui_hline(0, TASKBAR_Y, SCREEN_W, COL_ACCENT);   /* Akzentlinie oben */

    /* ── Start-Knopf ──────────────────────────────────────────── */
    uint32_t sbg = start_open ? COL_ACCENT : COL_PANEL2;
    uint32_t stx = start_open ? COL_BTN_TEXT : COL_ACCENT;
    ui_fill_rect(START_BTN_X, START_BTN_Y, START_BTN_W, START_BTN_H, sbg);
    ui_draw_rect(START_BTN_X, START_BTN_Y, START_BTN_W, START_BTN_H,
                 COL_ACCENT_DIM);
    /* Pinguin-Logo-Icon (klein, 16px) links im Knopf */
    /* Wir zeichnen es klein mit scale=1 (10×10) */
    for (int row = 0; row < 10; row++)
        for (int col = 0; col < 10; col++) {
            uint32_t c = ICON_PENGUIN[row * 10 + col];
            if (c == 0) c = sbg;
            /* Aufhellen für Start-Hintergrund */
            ui_put_pixel(START_BTN_X + 5 + col,
                          START_BTN_Y + 10 + row, c);
        }
    ui_draw_text(START_BTN_X + 18, START_BTN_Y + 11,
                  "Start", stx);

    /* ── Uhrzeit rechts ───────────────────────────────────────── */
    rtc_time_t t;
    rtc_read(&t);
    char timebuf[16];
    snprintf(timebuf, sizeof(timebuf), "%02u:%02u",
             t.hour, t.minute);
    int tw = (int)strlen(timebuf) * (FONT_W + 1);
    ui_draw_text(SCREEN_W - tw - 14, TASKBAR_Y + 14,
                  timebuf, COL_ACCENT);

    /* ── Benutzername rechts daneben ─────────────────────────── */
    int ulen = (int)strlen(username) * (FONT_W + 1);
    ui_draw_text(SCREEN_W - tw - ulen - 28, TASKBAR_Y + 14,
                  username, COL_GRAY);

    /* Trennpunkt */
    ui_draw_text(SCREEN_W - tw - ulen - 30, TASKBAR_Y + 14,
                  "|", COL_SEPARATOR);
}

/* ══════════════════════════════════════════════════════════════
 *  Startmenü
 * ══════════════════════════════════════════════════════════════ */
#define MENU_W    180
#define MENU_H    200
#define MENU_X    START_BTN_X
#define MENU_Y    (TASKBAR_Y - MENU_H)

typedef enum {
    MENU_NONE,
    MENU_LOGOUT,
    MENU_SHUTDOWN,
    MENU_ABOUT
} menu_action_t;

static const struct { const char *label; const uint32_t *icon; } menu_items[] = {
    { "Uber PinguinOS", ICON_INFO    },
    { "Einstellungen",  ICON_SETTINGS},
    { "Abmelden",       ICON_USER    },
    { "Herunterfahren", ICON_POWER   },
};
#define MENU_ITEM_COUNT 4
#define MENU_ITEM_H     44

static menu_action_t draw_menu_and_hit(int mx, int my, bool clicked)
{
    /* Schatten + Hintergrund */
    ui_draw_shadow_box(MENU_X, MENU_Y, MENU_W, MENU_H);

    /* Teal-Kopf */
    ui_fill_rect(MENU_X, MENU_Y, MENU_W, 30, 0x0A0A18);
    ui_hline(MENU_X, MENU_Y + 30, MENU_W, COL_ACCENT);
    ui_draw_text_centered(MENU_X, MENU_Y + 8, MENU_W, 14,
                           "PinguinOS", COL_ACCENT);

    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        int iy = MENU_Y + 34 + i * MENU_ITEM_H;
        bool hover = (mx >= MENU_X && mx < MENU_X + MENU_W &&
                      my >= iy     && my < iy + MENU_ITEM_H);

        uint32_t bg = hover ? COL_HOVER : COL_PANEL;
        ui_fill_rect(MENU_X + 1, iy, MENU_W - 2, MENU_ITEM_H - 1, bg);

        /* Icon (10×10, keine Skalierung) */
        for (int row = 0; row < 10; row++)
            for (int col = 0; col < 10; col++) {
                uint32_t c = menu_items[i].icon[row * 10 + col];
                if (c == 0) c = bg;
                ui_put_pixel(MENU_X + 8 + col, iy + 17 + row, c);
            }

        /* Label */
        ui_draw_text(MENU_X + 24, iy + 18,
                      menu_items[i].label,
                      hover ? COL_ACCENT : COL_WHITE);

        /* Trennlinie */
        ui_hline(MENU_X + 8, iy + MENU_ITEM_H - 1,
                  MENU_W - 16, COL_SEPARATOR);

        if (clicked && hover) {
            switch (i) {
            case 2: return MENU_LOGOUT;
            case 3: return MENU_SHUTDOWN;
            default: return MENU_NONE;
            }
        }
    }
    return MENU_NONE;
}

/* ══════════════════════════════════════════════════════════════
 *  Wallpaper
 * ══════════════════════════════════════════════════════════════ */
static void draw_wallpaper(void)
{
    ui_clear(COL_BG);

    /* Teal-Headerstreifen ganz oben */
    ui_fill_rect(0, 0, SCREEN_W, 4, COL_ACCENT);
    ui_hline(0, 5, SCREEN_W, COL_ACCENT_DIM);

    /* Subtiles Raster */
    for (int x = 0; x < SCREEN_W; x += 80)
        ui_vline(x, 8, TASKBAR_Y - 8, 0x0C0C1A);
    for (int y = 50; y < TASKBAR_Y; y += 80)
        ui_hline(0, y, SCREEN_W, 0x0C0C1A);

    /* "PinguinOS" Wasserzeichen (diagonal, gedimmt) */
    int wx = SCREEN_W / 2 - 50;
    int wy = SCREEN_H / 2 - 4;
    ui_draw_text(wx, wy, "PinguinOS 2.3", 0x111122);
}

/* ══════════════════════════════════════════════════════════════
 *  Mauszeiger (einfaches Kreuz)
 * ══════════════════════════════════════════════════════════════ */
static void draw_cursor(int x, int y)
{
    /* Kleines weißes Dreieck / Pfeil-Cursor */
    uint32_t c = COL_WHITE;
    for (int i = 0; i < 12; i++) {
        ui_put_pixel(x,   y+i, c);  /* links         */
        ui_put_pixel(x+i, y+i, c);  /* Diagonale     */
    }
    for (int i = 0; i < 6; i++)
        ui_put_pixel(x+6+i, y+6+i, c);   /* Schwanz       */
    /* Schwarze Outline */
    for (int i = 0; i < 12; i++) {
        ui_put_pixel(x-1,   y+i, 0);
        ui_put_pixel(x+1,   y+i, 0x0A0A0A);
        ui_put_pixel(x+i+1, y+i, 0x0A0A0A);
    }
}

/* ══════════════════════════════════════════════════════════════
 *  Haupt-Schleife
 * ══════════════════════════════════════════════════════════════ */
desktop_state_t desktop_run(const os_user_t *user)
{
    bool start_open     = false;
    desktop_state_t ret = DESKTOP_RUNNING;
    uint8_t prev_mouse_btn = 0;   /* Flanken-Erkennung für Klicks */

    mouse_set_bounds(SCREEN_W, SCREEN_H);

    while (ret == DESKTOP_RUNNING) {

        /* ── Maus-Events ───────────────────────────────────────── */
        mouse_event_t me;
        /* Alle Events aus der Queue lesen */
        while (mouse_poll(&me));

        int mx = mouse_get_x();
        int my = mouse_get_y();

        /* Klick = Taste war vorher offen, ist jetzt gedrückt (Flanke) */
        uint8_t cur_btn = mouse_get_buttons();
        bool clicked = (cur_btn & MOUSE_BTN_LEFT) &&
                       !(prev_mouse_btn & MOUSE_BTN_LEFT);
        prev_mouse_btn = cur_btn;

        /* ── Tastatur-Events ────────────────────────────────────── */
        kbd_event_t ke;
        while (kbd_poll(&ke)) {
            if (!ke.released && ke.scancode == KEY_ESCAPE)
                start_open = false;
        }

        /* ── Start-Knopf-Klick ──────────────────────────────────── */
        if (clicked) {
            if (mx >= START_BTN_X && mx < START_BTN_X + START_BTN_W &&
                my >= START_BTN_Y && my < START_BTN_Y + START_BTN_H) {
                start_open = !start_open;
                clicked    = false;   /* nicht weiterreichen */
            }
        }

        /* ── Desktop-Icon-Klick ─────────────────────────────────── */
        if (clicked && !start_open) {
            for (int i = 0; i < DESKTOP_ICON_COUNT; i++) {
                int ix = desktop_icons[i].x;
                int iy = desktop_icons[i].y;
                if (mx >= ix && mx < ix + ICON_TILE_W &&
                    my >= iy && my < iy + ICON_TILE_H) {
                    /* Alle deselektieren, dieses selektieren */
                    for (int j = 0; j < DESKTOP_ICON_COUNT; j++)
                        desktop_icons[j].selected = false;
                    desktop_icons[i].selected = true;
                    if (desktop_icons[i].on_click)
                        desktop_icons[i].on_click();
                }
            }
            /* Klick außerhalb → alle abwählen */
            bool on_icon = false;
            for (int i = 0; i < DESKTOP_ICON_COUNT && !on_icon; i++) {
                int ix = desktop_icons[i].x;
                int iy = desktop_icons[i].y;
                if (mx >= ix && mx < ix + ICON_TILE_W &&
                    my >= iy && my < iy + ICON_TILE_H)
                    on_icon = true;
            }
            if (!on_icon)
                for (int i = 0; i < DESKTOP_ICON_COUNT; i++)
                    desktop_icons[i].selected = false;
        }

        /* ════ Zeichnen ════════════════════════════════════════════ */

        /* Wallpaper + Icons */
        draw_wallpaper();

        for (int i = 0; i < DESKTOP_ICON_COUNT; i++) {
            ui_draw_icon_tile(desktop_icons[i].x,
                               desktop_icons[i].y,
                               desktop_icons[i].icon_pixels,
                               desktop_icons[i].label,
                               desktop_icons[i].selected);
        }

        /* Taskleiste */
        draw_taskbar(user->username, start_open);

        /* Startmenü (falls offen) */
        if (start_open) {
            menu_action_t action =
                draw_menu_and_hit(mx, my, clicked);
            switch (action) {
            case MENU_LOGOUT:
                ret = DESKTOP_LOGOUT;
                break;
            case MENU_SHUTDOWN:
                ret = DESKTOP_SHUTDOWN;
                break;
            default:
                break;
            }
        }

        /* Mauszeiger */
        draw_cursor(mx, my);

        ui_flip();
        task_sleep(16); /* ~60 FPS + Watchdog-Task bekommt regelmaessig CPU-Zeit */
    }

    return ret;
}