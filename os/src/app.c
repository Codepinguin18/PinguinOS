/**
 * @file app.c
 * @brief PinguinOS App-Verwaltung – Fenster, Builtins, PE-Laden.
 *
 * Verwaltet Fenster für Desktop-Apps und bietet eingebaute
 * Demo-Anwendungen (Dateien, Terminal, Rechner, Info).
 */

#include "../include/app.h"
#include "../include/ui.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/serial.h"
#include "../../kernal/include/heap.h"
#include "../../kernal/include/pe.h"
#include "../../kernal/include/keyboard.h"
#include "../../kernal/include/mouse.h"

/* ── Globale App-Instanzen ───────────────────────────────────────── */
static app_instance_t apps[APP_MAX_RUNNING];
static int next_win_x = 120;
static int next_win_y = 60;

/* ── Fenster-Konstanten ──────────────────────────────────────────── */
#define WIN_TITLE_H    28
#define WIN_BORDER      1
#define WIN_CLOSE_SIZE 18
#define WIN_MIN_W     200
#define WIN_MIN_H     120

/* ── Fenster-Farben ──────────────────────────────────────────────── */
#define WIN_BG         0x0E0E1E
#define WIN_TITLE_BG   0x0A0A18
#define WIN_TITLE_FOCUSED  0x141428
#define WIN_BORDER_COL 0x00FFF0
#define WIN_CLOSE_BG   0xFF4444
#define WIN_CLOSE_HOVER 0xFF6666
#define WIN_TEXT_COL   0xCCCCDD

/* ══════════════════════════════════════════════════════════════════
 *  Eingebaute Demo-Apps
 * ══════════════════════════════════════════════════════════════════ */

void builtin_files(app_window_t *win)
{
    strncpy(win->content,
        " Dateien in /\n"
        " ─────────────────────\n"
        "   /dev       [Geraete]\n"
        "   /proc      [Prozesse]\n"
        "   /sys       [System]\n"
        "   /mnt/disk  [Festplatte]\n"
        "   /tmp       [Temporaer]\n"
        "\n"
        " Klicke zum Oeffnen...\n",
        sizeof(win->content) - 1);
    win->content_lines = 9;
}

void builtin_terminal(app_window_t *win)
{
    strncpy(win->content,
        " PinguinOS Terminal v1.0\n"
        " ─────────────────────\n"
        " $ uname -a\n"
        "   PinguinOS 2.3 i686\n"
        " $ uptime\n"
        "   Laufzeit: aktiv\n"
        " $ help\n"
        "   Befehle: ls, cd, cat,\n"
        "   cp, mv, rm, clear\n"
        " $\n",
        sizeof(win->content) - 1);
    win->content_lines = 10;
}

void builtin_browser(app_window_t *win)
{
    strncpy(win->content,
        " Pinguin-Browser v1.0\n"
        " ─────────────────────\n"
        "   [Adressleiste: _____ ]\n"
        "\n"
        "   Willkommen im Web!\n"
        "\n"
        "   Netzwerk wird\n"
        "   konfiguriert...\n"
        "\n"
        "   (DHCP erforderlich)\n",
        sizeof(win->content) - 1);
    win->content_lines = 10;
}

void builtin_settings(app_window_t *win)
{
    strncpy(win->content,
        " Einstellungen\n"
        " ─────────────────────\n"
        "   Anzeige:    1280x720\n"
        "   Farbtiefe:  32 bpp\n"
        "   Maus:       USB-Tablet\n"
        "   Tastatur:   PS/2 DE\n"
        "   Netzwerk:   E1000/RTL\n"
        "   Swap:       16 MB\n"
        "   Heap:       verfuegbar\n",
        sizeof(win->content) - 1);
    win->content_lines = 9;
}

/* ══════════════════════════════════════════════════════════════════
 *  Initialisierung
 * ══════════════════════════════════════════════════════════════════ */

void app_init(void)
{
    memset(apps, 0, sizeof(apps));
    next_win_x = 120;
    next_win_y = 60;
    serial_puts("[App] App-System initialisiert\n");
}

/* ══════════════════════════════════════════════════════════════════
 *  Builtin starten
 * ══════════════════════════════════════════════════════════════════ */

int app_launch_builtin(const char *name, const char *title,
                       void (*fn)(app_window_t *win))
{
    /* Prüfen ob App schon läuft – dann fokussieren */
    for (int i = 0; i < APP_MAX_RUNNING; i++) {
        if (apps[i].running && strcmp(apps[i].name, name) == 0) {
            /* Fokus auf existierendes Fenster */
            for (int j = 0; j < APP_MAX_RUNNING; j++)
                apps[j].window.focused = false;
            apps[i].window.focused = true;
            apps[i].window.visible = true;
            serial_printf("[App] '%s' schon offen, fokussiert\n", name);
            return i;
        }
    }

    /* Freien Slot finden */
    int slot = -1;
    for (int i = 0; i < APP_MAX_RUNNING; i++) {
        if (!apps[i].running) { slot = i; break; }
    }
    if (slot < 0) {
        serial_puts("[App] FEHLER: Maximale App-Anzahl erreicht\n");
        return -1;
    }

    /* App-Instanz initialisieren */
    memset(&apps[slot], 0, sizeof(apps[slot]));
    strncpy(apps[slot].name, name, APP_NAME_MAX - 1);
    apps[slot].type       = APP_TYPE_BUILTIN;
    apps[slot].running    = true;
    apps[slot].builtin_fn = fn;

    /* Fenster einrichten */
    app_window_t *win = &apps[slot].window;
    win->x = next_win_x;
    win->y = next_win_y;
    win->w = 420;
    win->h = 300;
    strncpy(win->title, title, APP_NAME_MAX - 1);
    win->visible     = true;
    win->focused     = true;
    win->bg_color    = WIN_BG;
    win->title_color = WIN_BORDER_COL;

    /* Nächstes Fenster versetzt */
    next_win_x += 30;
    next_win_y += 25;
    if (next_win_x > 500) next_win_x = 120;
    if (next_win_y > 350) next_win_y = 60;

    /* Alle anderen defokussieren */
    for (int i = 0; i < APP_MAX_RUNNING; i++)
        if (i != slot) apps[i].window.focused = false;

    /* Builtin-Inhalt generieren */
    if (fn) fn(win);

    serial_printf("[App] '%s' gestartet in Slot %d\n", name, slot);
    return slot;
}

/* ══════════════════════════════════════════════════════════════════
 *  PE/EXE starten (Placeholder – liest Datei über VFS)
 * ══════════════════════════════════════════════════════════════════ */

int app_launch_exe(const char *path)
{
    serial_printf("[App] EXE laden: %s\n", path);

    /* VFS-Datei öffnen (gibt vfs_node_t* zurück) */
    extern void *vfs_open(const char *, uint32_t);
    extern int32_t vfs_read(void *node, uint32_t offset,
                            uint32_t size, uint8_t *buf);
    extern void vfs_close(void *node);

    void *node = vfs_open(path, 0);
    if (!node) {
        serial_printf("[App] FEHLER: Kann '%s' nicht oeffnen\n", path);
        return -1;
    }

    /* Einfache Implementierung: bis zu 1 MB lesen */
    uint32_t max_size = 1024 * 1024;
    uint8_t *buf = (uint8_t *)kmalloc(max_size);
    if (!buf) {
        vfs_close(node);
        serial_puts("[App] FEHLER: Nicht genug Speicher fuer EXE\n");
        return -1;
    }

    int32_t bytes = vfs_read(node, 0, max_size, buf);
    vfs_close(node);

    if (bytes <= 0) {
        kfree(buf);
        serial_printf("[App] FEHLER: Kann '%s' nicht lesen\n", path);
        return -1;
    }

    /* Freien Slot finden */
    int slot = -1;
    for (int i = 0; i < APP_MAX_RUNNING; i++) {
        if (!apps[i].running) { slot = i; break; }
    }
    if (slot < 0) {
        kfree(buf);
        serial_puts("[App] FEHLER: Maximale App-Anzahl erreicht\n");
        return -1;
    }

    /* PE laden */
    memset(&apps[slot], 0, sizeof(apps[slot]));
    int err = pe_load(buf, (uint32_t)bytes, path, &apps[slot].pe_image);
    kfree(buf);   /* Raw-Buffer nicht mehr nötig */

    if (err != 0) {
        serial_printf("[App] PE-Ladefehler: %d\n", err);
        return -1;
    }

    strncpy(apps[slot].name, path, APP_NAME_MAX - 1);
    apps[slot].type    = APP_TYPE_PE_EXE;
    apps[slot].running = true;

    /* Fenster einrichten */
    app_window_t *win = &apps[slot].window;
    win->x = next_win_x;
    win->y = next_win_y;
    win->w = 420;
    win->h = 300;
    strncpy(win->title, path, APP_NAME_MAX - 1);
    win->visible     = true;
    win->focused     = true;
    win->bg_color    = WIN_BG;
    win->title_color = WIN_BORDER_COL;
    snprintf(win->content, sizeof(win->content),
             " EXE geladen: %s\n EP: 0x%08X\n Groesse: %u Bytes\n",
             path, apps[slot].pe_image.entry_point,
             apps[slot].pe_image.image_size);
    win->content_lines = 3;

    next_win_x += 30;
    next_win_y += 25;

    serial_printf("[App] EXE '%s' geladen in Slot %d\n", path, slot);
    return slot;
}

/* ══════════════════════════════════════════════════════════════════
 *  App schließen
 * ══════════════════════════════════════════════════════════════════ */

void app_close(int index)
{
    if (index < 0 || index >= APP_MAX_RUNNING) return;
    if (!apps[index].running) return;

    serial_printf("[App] Schliesse '%s'\n", apps[index].name);

    if (apps[index].type == APP_TYPE_PE_EXE) {
        pe_unload(&apps[index].pe_image);
    }

    apps[index].running = false;
    apps[index].window.visible = false;

    /* Nächstes sichtbares Fenster fokussieren */
    for (int i = APP_MAX_RUNNING - 1; i >= 0; i--) {
        if (apps[i].running && apps[i].window.visible) {
            apps[i].window.focused = true;
            break;
        }
    }
}

/* ══════════════════════════════════════════════════════════════════
 *  Fenster zeichnen
 * ══════════════════════════════════════════════════════════════════ */

static void draw_window(app_window_t *win)
{
    if (!win->visible) return;

    int x = win->x, y = win->y, w = win->w, h = win->h;

    /* Schatten */
    ui_fill_rect(x + 4, y + 4, w, h, 0x02020A);
    ui_fill_rect(x + 2, y + 2, w, h, 0x05050E);

    /* Hauptkörper */
    ui_fill_rect(x, y, w, h, win->bg_color);

    /* Rahmen */
    uint32_t border = win->focused ? WIN_BORDER_COL : 0x444466;
    ui_draw_rect(x, y, w, h, border);

    /* Titelleiste */
    uint32_t title_bg = win->focused ? WIN_TITLE_FOCUSED : WIN_TITLE_BG;
    ui_fill_rect(x + 1, y + 1, w - 2, WIN_TITLE_H, title_bg);
    ui_hline(x, y + WIN_TITLE_H, w, border);

    /* Titel-Text */
    uint32_t tcol = win->focused ? win->title_color : 0x666688;
    ui_draw_text(x + 10, y + 10, win->title, tcol);

    /* Schließen-Knopf (X) */
    int cx = x + w - WIN_CLOSE_SIZE - 6;
    int cy = y + 5;
    ui_fill_rect(cx, cy, WIN_CLOSE_SIZE, WIN_CLOSE_SIZE, WIN_CLOSE_BG);
    ui_draw_rect(cx, cy, WIN_CLOSE_SIZE, WIN_CLOSE_SIZE, 0xCC3333);
    /* X zeichnen */
    for (int i = 3; i < WIN_CLOSE_SIZE - 3; i++) {
        ui_put_pixel(cx + i, cy + i, COL_WHITE);
        ui_put_pixel(cx + WIN_CLOSE_SIZE - 1 - i, cy + i, COL_WHITE);
    }

    /* Inhalt */
    if (win->content[0]) {
        int tx = x + 10;
        int ty = y + WIN_TITLE_H + 10;
        const char *p = win->content;
        while (*p && ty < y + h - 12) {
            /* Zeile finden */
            const char *nl = p;
            while (*nl && *nl != '\n') nl++;
            /* Zeile zeichnen (Zeichen für Zeichen) */
            int lx = tx;
            for (const char *c = p; c < nl && lx < x + w - 10; c++) {
                lx = ui_draw_char(lx, ty, *c, WIN_TEXT_COL);
            }
            ty += 12;
            p = (*nl == '\n') ? nl + 1 : nl;
        }
    }
}

void app_draw_windows(void)
{
    /* Zeichne nicht-fokussierte Fenster zuerst */
    for (int i = 0; i < APP_MAX_RUNNING; i++)
        if (apps[i].running && !apps[i].window.focused)
            draw_window(&apps[i].window);

    /* Fokussiertes Fenster zuletzt (vorne) */
    for (int i = 0; i < APP_MAX_RUNNING; i++)
        if (apps[i].running && apps[i].window.focused)
            draw_window(&apps[i].window);
}

/* ══════════════════════════════════════════════════════════════════
 *  Maus-Verarbeitung
 * ══════════════════════════════════════════════════════════════════ */

bool app_handle_mouse(int mx, int my, uint8_t buttons,
                      uint8_t clicked, uint8_t released)
{
    /* Drag-Handling: laufende Drags weiterführen */
    for (int i = 0; i < APP_MAX_RUNNING; i++) {
        app_window_t *win = &apps[i].window;
        if (!apps[i].running || !win->visible) continue;

        if (win->dragging) {
            if (buttons & MOUSE_BTN_LEFT) {
                /* Fenster mitbewegen */
                win->x = mx - win->drag_ox;
                win->y = my - win->drag_oy;
                /* Clamp */
                if (win->x < 0) win->x = 0;
                if (win->y < 0) win->y = 0;
                if (win->x + win->w > SCREEN_W) win->x = SCREEN_W - win->w;
                if (win->y + win->h > TASKBAR_Y) win->y = TASKBAR_Y - win->h;
                return true;
            } else {
                win->dragging = false;
            }
        }
    }

    /* Klick-Verarbeitung (von vorne nach hinten / fokussiert zuerst) */
    if (clicked & MOUSE_BTN_LEFT) {
        /* Zuerst fokussiertes Fenster prüfen */
        for (int pass = 0; pass < 2; pass++) {
            for (int i = APP_MAX_RUNNING - 1; i >= 0; i--) {
                app_window_t *win = &apps[i].window;
                if (!apps[i].running || !win->visible) continue;
                if (pass == 0 && !win->focused) continue;
                if (pass == 1 &&  win->focused) continue;

                /* Hit-Test */
                if (mx >= win->x && mx < win->x + win->w &&
                    my >= win->y && my < win->y + win->h)
                {
                    /* Schließen-Knopf? */
                    int cx = win->x + win->w - WIN_CLOSE_SIZE - 6;
                    int cy = win->y + 5;
                    if (mx >= cx && mx < cx + WIN_CLOSE_SIZE &&
                        my >= cy && my < cy + WIN_CLOSE_SIZE) {
                        app_close(i);
                        return true;
                    }

                    /* Titelleiste → Drag starten */
                    if (my < win->y + WIN_TITLE_H) {
                        win->dragging = true;
                        win->drag_ox = mx - win->x;
                        win->drag_oy = my - win->y;
                    }

                    /* Fokus setzen */
                    for (int j = 0; j < APP_MAX_RUNNING; j++)
                        apps[j].window.focused = false;
                    win->focused = true;
                    return true;
                }
            }
        }
    }

    return false;
}

/* ══════════════════════════════════════════════════════════════════
 *  Tastatur
 * ══════════════════════════════════════════════════════════════════ */

void app_handle_key(uint8_t scancode, bool released_key)
{
    /* TODO: Tastatureingabe an fokussiertes Fenster weiterleiten */
    (void)scancode;
    (void)released_key;
}

/* ── Hilfsfunktionen ─────────────────────────────────────────────── */

int app_running_count(void)
{
    int cnt = 0;
    for (int i = 0; i < APP_MAX_RUNNING; i++)
        if (apps[i].running) cnt++;
    return cnt;
}

app_instance_t *app_get_instance(int index)
{
    if (index < 0 || index >= APP_MAX_RUNNING) return (void *)0;
    return &apps[index];
}
