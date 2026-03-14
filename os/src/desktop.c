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
 *   – App-Fenster (Dateien, Terminal, Browser, Optionen)
 *   – Notification-Toasts bei Aktionen
 *   – Rechtsklick-Kontextmenü
 */

#include "../include/desktop.h"
#include "../include/ui.h"
#include "../include/app.h"
#include "icons.h"
#include "../../kernal/include/keyboard.h"
#include "../../kernal/include/mouse.h"
#include "../../kernal/include/rtc.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/scheduler.h"
#include "../../kernal/include/serial.h"

/* ══════════════════════════════════════════════════════════════
 *  Desktop-Icon-Layout
 * ══════════════════════════════════════════════════════════════ */
#define ICON_COLS       2
#define ICON_TILE_W     70
#define ICON_TILE_H     85
#define ICON_START_X    24
#define ICON_START_Y    40

/* ── App-Handler ─────────────────────────────────────────────── */
/* Forward-Deklarationen der Builtin-Funktionen aus app.c */
extern void builtin_files(app_window_t *win);
extern void builtin_terminal(app_window_t *win);
extern void builtin_browser(app_window_t *win);
extern void builtin_settings(app_window_t *win);

static void action_open_files(void)
{
    app_launch_builtin("files", "Dateien", builtin_files);
}

static void action_open_terminal(void)
{
    app_launch_builtin("terminal", "Terminal", builtin_terminal);
}

static void action_open_browser(void)
{
    app_launch_builtin("browser", "Pinguin-Browser", builtin_browser);
}

static void action_open_settings(void)
{
    app_launch_builtin("settings", "Einstellungen", builtin_settings);
}

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
 *  Notification Toast
 * ══════════════════════════════════════════════════════════════ */
#define TOAST_DURATION_MS  2500
#define TOAST_W            260
#define TOAST_H            36
#define TOAST_X            (SCREEN_W - TOAST_W - 16)
#define TOAST_Y            (TASKBAR_Y - TOAST_H - 12)

static char   toast_msg[64];
static uint32_t toast_end_time = 0;
static uint32_t toast_color    = COL_ACCENT;

static void toast_show(const char *msg, uint32_t color)
{
    strncpy(toast_msg, msg, sizeof(toast_msg) - 1);
    toast_msg[sizeof(toast_msg) - 1] = '\0';
    toast_end_time = sched_uptime_ms() + TOAST_DURATION_MS;
    toast_color = color;
}

static void draw_toast(void)
{
    if (toast_end_time == 0 || sched_uptime_ms() > toast_end_time) return;

    /* Hintergrund mit Fade */
    ui_fill_rect(TOAST_X - 2, TOAST_Y - 2, TOAST_W + 4, TOAST_H + 4, 0x05050E);
    ui_fill_rect(TOAST_X, TOAST_Y, TOAST_W, TOAST_H, COL_PANEL2);
    ui_draw_rect(TOAST_X, TOAST_Y, TOAST_W, TOAST_H, toast_color);

    /* Akzent-Linie links */
    ui_fill_rect(TOAST_X, TOAST_Y, 3, TOAST_H, toast_color);

    /* Text */
    ui_draw_text(TOAST_X + 12, TOAST_Y + 14, toast_msg, COL_WHITE);
}

/* ══════════════════════════════════════════════════════════════
 *  Rechtsklick-Kontextmenü
 * ══════════════════════════════════════════════════════════════ */
#define CTX_W     160
#define CTX_H     132
#define CTX_ITEM_H 28
static bool ctx_open  = false;
static int  ctx_x     = 0;
static int  ctx_y     = 0;

static const char *ctx_items[] = {
    "Aktualisieren",
    "Neuer Ordner",
    "Terminal hier",
    "Info anzeigen",
};
#define CTX_ITEM_COUNT 4

static void draw_context_menu(int mx, int my)
{
    if (!ctx_open) return;

    /* Schatten */
    ui_fill_rect(ctx_x + 3, ctx_y + 3, CTX_W, CTX_H, 0x02020A);
    ui_fill_rect(ctx_x, ctx_y, CTX_W, CTX_H, COL_PANEL);
    ui_draw_rect(ctx_x, ctx_y, CTX_W, CTX_H, COL_ACCENT_DIM);

    for (int i = 0; i < CTX_ITEM_COUNT; i++) {
        int iy = ctx_y + 6 + i * CTX_ITEM_H;
        bool hover = (mx >= ctx_x && mx < ctx_x + CTX_W &&
                      my >= iy     && my < iy + CTX_ITEM_H);
        if (hover)
            ui_fill_rect(ctx_x + 2, iy, CTX_W - 4, CTX_ITEM_H, COL_HOVER);
        ui_draw_text(ctx_x + 14, iy + 10, ctx_items[i],
                     hover ? COL_ACCENT : COL_WHITE);
        if (i < CTX_ITEM_COUNT - 1)
            ui_hline(ctx_x + 8, iy + CTX_ITEM_H - 1, CTX_W - 16, COL_SEPARATOR);
    }
}

static int ctx_hit_test(int mx, int my)
{
    if (!ctx_open) return -1;
    if (mx < ctx_x || mx >= ctx_x + CTX_W) return -1;
    for (int i = 0; i < CTX_ITEM_COUNT; i++) {
        int iy = ctx_y + 6 + i * CTX_ITEM_H;
        if (my >= iy && my < iy + CTX_ITEM_H)
            return i;
    }
    return -1;
}

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
    for (int row = 0; row < 10; row++)
        for (int col = 0; col < 10; col++) {
            uint32_t c = ICON_PENGUIN[row * 10 + col];
            if (c == 0) c = sbg;
            ui_put_pixel(START_BTN_X + 5 + col,
                          START_BTN_Y + 10 + row, c);
        }
    ui_draw_text(START_BTN_X + 18, START_BTN_Y + 11,
                  "Start", stx);

    /* ── App-Knöpfe in der Taskleiste ─────────────────────────── */
    int tab_x = START_BTN_X + START_BTN_W + 10;
    for (int i = 0; i < APP_MAX_RUNNING; i++) {
        app_instance_t *inst = app_get_instance(i);
        if (!inst || !inst->running) continue;
        int tw = 100;
        uint32_t tb_bg = inst->window.focused ? 0x1A1A30 : COL_PANEL2;
        uint32_t tb_fg = inst->window.focused ? COL_ACCENT : COL_GRAY;
        ui_fill_rect(tab_x, TASKBAR_Y + 8, tw, 24, tb_bg);
        ui_draw_rect(tab_x, TASKBAR_Y + 8, tw, 24,
                     inst->window.focused ? COL_ACCENT_DIM : COL_DARK_GRAY);
        /* Abschneiden des Namens */
        char short_name[14];
        strncpy(short_name, inst->window.title, 13);
        short_name[13] = '\0';
        ui_draw_text(tab_x + 6, TASKBAR_Y + 16, short_name, tb_fg);
        tab_x += tw + 4;
        if (tab_x > SCREEN_W - 250) break;
    }

    /* ── Uhrzeit rechts ───────────────────────────────────────── */
    rtc_time_t t;
    rtc_read(&t);
    char timebuf[16];
    snprintf(timebuf, sizeof(timebuf), "%02u:%02u",
             t.hour, t.minute);
    int twid = (int)strlen(timebuf) * (FONT_W + 1);
    ui_draw_text(SCREEN_W - twid - 14, TASKBAR_Y + 14,
                  timebuf, COL_ACCENT);

    /* ── Benutzername rechts daneben ─────────────────────────── */
    int ulen = (int)strlen(username) * (FONT_W + 1);
    ui_draw_text(SCREEN_W - twid - ulen - 28, TASKBAR_Y + 14,
                  username, COL_GRAY);

    /* Trennpunkt */
    ui_draw_text(SCREEN_W - twid - ulen - 30, TASKBAR_Y + 14,
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
            case 0: /* Über PinguinOS */
                toast_show("PinguinOS 2.3  |  x86-32  |  PE-Support", COL_ACCENT);
                return MENU_NONE;
            case 1: /* Einstellungen */
                action_open_settings();
                return MENU_NONE;
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
 *  Mauszeiger (Pfeil-Cursor)
 * ══════════════════════════════════════════════════════════════ */
static void draw_cursor(int x, int y)
{
    uint32_t c = COL_WHITE;
    for (int i = 0; i < 12; i++) {
        ui_put_pixel(x,   y+i, c);
        ui_put_pixel(x+i, y+i, c);
    }
    for (int i = 0; i < 6; i++)
        ui_put_pixel(x+6+i, y+6+i, c);
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
    uint8_t prev_mouse_btn = 0;

    mouse_set_bounds(SCREEN_W, SCREEN_H);
    app_init();

    while (ret == DESKTOP_RUNNING) {

        /* ── Maus-Events ───────────────────────────────────────── */
        mouse_event_t me;
        uint8_t last_clicked  = 0;
        uint8_t last_released = 0;
        /* Alle Events aus der Queue lesen, letzten Klick-Status merken */
        while (mouse_poll(&me)) {
            last_clicked  |= me.clicked;
            last_released |= me.released;
        }

        int mx = mouse_get_x();
        int my = mouse_get_y();

        /* Klick = Taste war vorher offen, ist jetzt gedrückt (Flanke) */
        uint8_t cur_btn = mouse_get_buttons();
        bool clicked     = (last_clicked & MOUSE_BTN_LEFT) != 0;
        bool right_click = (last_clicked & MOUSE_BTN_RIGHT) != 0;
        prev_mouse_btn = cur_btn;

        /* ── Tastatur-Events ────────────────────────────────────── */
        kbd_event_t ke;
        while (kbd_poll(&ke)) {
            if (!ke.released && ke.scancode == KEY_ESCAPE) {
                start_open = false;
                ctx_open   = false;
            }
            app_handle_key(ke.scancode, ke.released);
        }

        /* ── Rechtsklick-Kontextmenü ────────────────────────────── */
        if (right_click && !start_open) {
            /* Nur auf dem Desktop (nicht auf Fenstern/Icons) */
            bool on_any = false;
            for (int i = 0; i < DESKTOP_ICON_COUNT; i++) {
                int ix = desktop_icons[i].x;
                int iy = desktop_icons[i].y;
                if (mx >= ix && mx < ix + ICON_TILE_W &&
                    my >= iy && my < iy + ICON_TILE_H)
                    on_any = true;
            }
            if (!on_any && my < TASKBAR_Y) {
                ctx_open = true;
                ctx_x = mx;
                ctx_y = my;
                /* Clamp zum Bildschirm */
                if (ctx_x + CTX_W > SCREEN_W) ctx_x = SCREEN_W - CTX_W;
                if (ctx_y + CTX_H > TASKBAR_Y) ctx_y = TASKBAR_Y - CTX_H;
            }
        }

        /* ── Kontextmenü-Klick ──────────────────────────────────── */
        if (clicked && ctx_open) {
            int hit = ctx_hit_test(mx, my);
            ctx_open = false;
            if (hit >= 0) {
                switch (hit) {
                case 0: toast_show("Desktop aktualisiert", COL_SUCCESS); break;
                case 1: toast_show("Ordner erstellt", COL_ACCENT); break;
                case 2: action_open_terminal(); break;
                case 3:
                    toast_show("PinguinOS 2.3  |  PE/EXE Support", COL_ACCENT);
                    break;
                }
                clicked = false;
            }
        }

        /* ── App-Fenster Maus-Verarbeitung ──────────────────────── */
        if (app_handle_mouse(mx, my, cur_btn, last_clicked, last_released)) {
            /* Fenster hat Event verarbeitet, nicht weiterreichen */
            clicked = false;
        }

        /* ── Start-Knopf-Klick ──────────────────────────────────── */
        if (clicked) {
            if (mx >= START_BTN_X && mx < START_BTN_X + START_BTN_W &&
                my >= START_BTN_Y && my < START_BTN_Y + START_BTN_H) {
                start_open = !start_open;
                clicked    = false;
            }
        }

        /* ── Startmenü außerhalb-Klick ──────────────────────────── */
        if (clicked && start_open) {
            if (!(mx >= MENU_X && mx < MENU_X + MENU_W &&
                  my >= MENU_Y && my < MENU_Y + MENU_H) &&
                !(mx >= START_BTN_X && mx < START_BTN_X + START_BTN_W &&
                  my >= START_BTN_Y && my < START_BTN_Y + START_BTN_H)) {
                start_open = false;
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
                    if (desktop_icons[i].on_click) {
                        desktop_icons[i].on_click();
                        toast_show(desktop_icons[i].label, COL_ACCENT);
                    }
                    clicked = false;
                    break;
                }
            }

            /* Klick außerhalb → alle abwählen */
            if (clicked) {
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

        /* App-Fenster */
        app_draw_windows();

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

        /* Kontextmenü */
        draw_context_menu(mx, my);

        /* Notification Toast */
        draw_toast();

        /* Mauszeiger */
        draw_cursor(mx, my);

        ui_flip();
        task_sleep(16); /* ~60 FPS */
    }

    return ret;
}