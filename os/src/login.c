/**
 * @file login.c
 * @brief PinguinOS Login-Bildschirm + Erster-Start-Setup.
 *
 * Fixes:
 *  - task_sleep(16) in allen Schleifen → Watchdog läuft, kein Timeout-Crash
 *  - Mausklick-Erkennung: Klick auf Feld → Fokus + Konsolen-Log
 *  - Pfeiltasten HOCH/RUNTER wechseln zwischen Feldern
 *  - Klick auf "Erstellen"/"Anmelden"-Button löst Aktion aus
 *  - Hover-Effekt auf Schaltfläche
 */

#include "../include/login.h"
#include "../include/ui.h"
#include "icons.h"
#include "../../kernal/include/keyboard.h"
#include "../../kernal/include/mouse.h"
#include "../../kernal/include/serial.h"
#include "../../kernal/include/rtc.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/scheduler.h"

/* ── Benutzer-Datenbank (in-memory) ─────────────────────────── */
static os_user_t  user_db[LOGIN_MAX_USERS];
static int        user_count = 0;

bool users_add(const char *username, const char *password, bool admin)
{
    if (user_count >= LOGIN_MAX_USERS) return false;
    strncpy(user_db[user_count].username, username, LOGIN_USERNAME_MAX - 1);
    strncpy(user_db[user_count].password_hash, password, LOGIN_PASSWORD_MAX - 1);
    user_db[user_count].admin = admin;
    user_count++;
    return true;
}

static bool check_login(const char *user, const char *pass, os_user_t *out)
{
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, user) == 0 &&
            strcmp(user_db[i].password_hash, pass) == 0) {
            *out = user_db[i];
            return true;
        }
    }
    return false;
}

/* ══════════════════════════════════════════════════════════════
 *  UI-Konstanten  (vor allen draw_*-Funktionen!)
 * ══════════════════════════════════════════════════════════════ */

/* Gemeinsame Maße */
#define FIELD_H   36
#define BTN_W    140
#define BTN_H     36

/* ── Login-Karte ── */
#define CARD_W   380
#define CARD_H   340
#define CARD_X   ((SCREEN_W - CARD_W) / 2)
#define CARD_Y   ((SCREEN_H - CARD_H) / 2 - 20)
#define FIELD_W  300
#define FIELD_X  (CARD_X + (CARD_W - FIELD_W) / 2)
#define USER_Y   (CARD_Y + 170)
#define PASS_Y   (CARD_Y + 220)
#define BTN_Y    (CARD_Y + 275)
#define LOGIN_BTN_X  (CARD_X + (CARD_W - BTN_W) / 2)

/* ── Setup-Karte ── */
#define SETUP_W   380
#define SETUP_H   400
#define SETUP_X   ((SCREEN_W - SETUP_W) / 2)
#define SETUP_Y   ((SCREEN_H - SETUP_H) / 2 - 10)
#define SF_W      300
#define SF_X      (SETUP_X + (SETUP_W - SF_W) / 2)
#define SU_Y      (SETUP_Y + 150)
#define SP_Y      (SETUP_Y + 210)
#define SP2_Y     (SETUP_Y + 270)
#define SB_Y      (SETUP_Y + 340)
#define SETUP_BTN_X  (SETUP_X + (SETUP_W - BTN_W) / 2)

/* ── Hilfsmakro: Hit-Test ── */
static bool hit(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

/* ── Hintergrund ────────────────────────────────────────────── */
static void draw_background(void)
{
    ui_clear(COL_BG);
    ui_fill_rect(0, 0, SCREEN_W, 6, COL_ACCENT);
    ui_hline(0, 8, SCREEN_W, COL_ACCENT_DIM);
    for (int i = 0; i < SCREEN_W; i += 40)
        ui_vline(i, TASKBAR_Y - 80, 80, 0x0C0C1C);
}

/* ── Uhrzeit ────────────────────────────────────────────────── */
static void draw_clock(void)
{
    rtc_time_t t;
    rtc_read(&t);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02u:%02u  %02u.%02u.%04u",
             t.hour, t.minute, t.day, t.month,
             (uint32_t)(2000 + t.year));
    int len = (int)strlen(buf) * (FONT_W + 1);
    ui_draw_text(SCREEN_W - len - 16, 14, buf, COL_GRAY);
}

/* ── Login-Karte ────────────────────────────────────────────── */
static void draw_card(const char *username, const char *password,
                      int focus, bool error, const char *error_msg,
                      int mx, int my)
{
    ui_draw_shadow_box(CARD_X, CARD_Y, CARD_W, CARD_H);
    ui_fill_rect(CARD_X, CARD_Y, CARD_W, 50, 0x0A0A18);
    ui_hline(CARD_X, CARD_Y + 50, CARD_W, COL_ACCENT);
    ui_draw_icon(CARD_X + 14, CARD_Y + 9, ICON_PENGUIN);
    ui_draw_text(CARD_X + 56, CARD_Y + 16, "PinguinOS", COL_ACCENT);
    ui_draw_text(CARD_X + 56 + 9 * (FONT_W + 1) + 4, CARD_Y + 16, "2.3", COL_WHITE);
    ui_draw_text_centered(CARD_X, CARD_Y + 58, CARD_W, 16, "Bitte anmelden", COL_GRAY);

    ui_draw_text(FIELD_X, USER_Y - 16, "Benutzername", COL_GRAY);
    ui_draw_input(FIELD_X, USER_Y, FIELD_W, FIELD_H, username, focus == 0, false);

    ui_draw_text(FIELD_X, PASS_Y - 16, "Passwort", COL_GRAY);
    ui_draw_input(FIELD_X, PASS_Y, FIELD_W, FIELD_H, password, focus == 1, true);

    if (error && error_msg)
        ui_draw_text_centered(CARD_X, CARD_Y + 262, CARD_W, 12, error_msg, COL_ERROR);

    bool btn_hover = hit(mx, my, LOGIN_BTN_X, BTN_Y, BTN_W, BTN_H);
    ui_draw_button(LOGIN_BTN_X, BTN_Y, BTN_W, BTN_H, "Anmelden", btn_hover);

    ui_draw_separator(CARD_X + 16, CARD_Y + CARD_H - 24, CARD_W - 32);
    ui_draw_text_centered(CARD_X, CARD_Y + CARD_H - 18, CARD_W, 14,
                          "PinguinOS v2.3  -  Secure Boot", COL_DARK_GRAY);
}

/* ── Setup-Karte (Erster-Start) ─────────────────────────────── */
static void draw_setup_card(const char *username, const char *password,
                             const char *password2, int focus,
                             bool error, const char *error_msg,
                             int mx, int my)
{
    ui_clear(COL_BG);
    ui_fill_rect(0, 0, SCREEN_W, 6, COL_ACCENT);
    ui_draw_shadow_box(SETUP_X, SETUP_Y, SETUP_W, SETUP_H);
    ui_fill_rect(SETUP_X, SETUP_Y, SETUP_W, 50, 0x0A0A18);
    ui_hline(SETUP_X, SETUP_Y + 50, SETUP_W, COL_ACCENT);
    ui_draw_icon(SETUP_X + 14, SETUP_Y + 9, ICON_PENGUIN);
    ui_draw_text(SETUP_X + 56, SETUP_Y + 16, "PinguinOS", COL_ACCENT);
    ui_draw_text(SETUP_X + 56 + 9 * (FONT_W + 1) + 4, SETUP_Y + 16, "2.3", COL_WHITE);
    ui_draw_text_centered(SETUP_X, SETUP_Y + 58, SETUP_W, 16,
                          "Ersten Benutzer anlegen", COL_GRAY);
    ui_draw_text_centered(SETUP_X, SETUP_Y + 80, SETUP_W, 12,
                          "Willkommen! Bitte Konto erstellen.", COL_ACCENT);

    ui_draw_text(SF_X, SU_Y  - 16, "Benutzername",         COL_GRAY);
    ui_draw_input(SF_X, SU_Y,  SF_W, FIELD_H, username,  focus == 0, false);

    ui_draw_text(SF_X, SP_Y  - 16, "Passwort",             COL_GRAY);
    ui_draw_input(SF_X, SP_Y,  SF_W, FIELD_H, password,  focus == 1, true);

    ui_draw_text(SF_X, SP2_Y - 16, "Passwort wiederholen", COL_GRAY);
    ui_draw_input(SF_X, SP2_Y, SF_W, FIELD_H, password2, focus == 2, true);

    if (error && error_msg)
        ui_draw_text_centered(SETUP_X, SETUP_Y + 315, SETUP_W, 12, error_msg, COL_ERROR);

    bool btn_hover = hit(mx, my, SETUP_BTN_X, SB_Y, BTN_W, BTN_H);
    ui_draw_button(SETUP_BTN_X, SB_Y, BTN_W, BTN_H, "Erstellen", btn_hover);

    ui_flip();
}

/* ── Fokus-Wechsel mit Log ───────────────────────────────────── */
static void set_focus(int *focus, int new_focus, int max_fields,
                      const char *field_names[])
{
    if (new_focus < 0)          new_focus = max_fields - 1;
    if (new_focus >= max_fields) new_focus = 0;
    if (*focus == new_focus)    return;
    *focus = new_focus;
    serial_puts("[Login] Feld ausgewaehlt: ");
    serial_puts(field_names[new_focus]);
    serial_puts("\n");
}

/* ── Setup-Aktion ausführen ─────────────────────────────────── */
static bool try_create_user(const char *username, const char *password,
                             const char *password2,
                             bool *error, char *error_msg, int emsg_len)
{
    if (strlen(username) < 1) {
        *error = true;
        strncpy(error_msg, "Benutzername darf nicht leer sein", emsg_len - 1);
        return false;
    }
    if (strlen(password) < 1) {
        *error = true;
        strncpy(error_msg, "Passwort darf nicht leer sein", emsg_len - 1);
        return false;
    }
    if (strcmp(password, password2) != 0) {
        *error = true;
        strncpy(error_msg, "Passwoerter stimmen nicht ueberein", emsg_len - 1);
        return false;
    }
    users_add(username, password, true);
    serial_puts("[Login] Benutzer erstellt: ");
    serial_puts(username);
    serial_puts("\n");
    return true;
}

/* ── Erster-Start-Setup ─────────────────────────────────────── */
static void first_run_setup(void)
{
    char username[LOGIN_USERNAME_MAX]  = {0};
    char password[LOGIN_PASSWORD_MAX]  = {0};
    char password2[LOGIN_PASSWORD_MAX] = {0};
    int  focus  = 0;
    bool error  = false;
    char error_msg[64] = {0};
    uint8_t prev_btn = 0;

    const char *field_names[] = { "Benutzername", "Passwort", "Passwort wiederholen" };

    serial_puts("[Login] Erster-Start: Benutzer-Erstellungsformular\n");
    serial_puts("[Login] Feld ausgewaehlt: Benutzername\n");

    for (;;) {
        /* Mausposition + Klick-Flanke */
        mouse_event_t me;
        while (mouse_poll(&me));
        int mx = mouse_get_x();
        int my = mouse_get_y();
        uint8_t cur_btn = mouse_get_buttons();
        bool clicked = (cur_btn & MOUSE_BTN_LEFT) && !(prev_btn & MOUSE_BTN_LEFT);
        prev_btn = cur_btn;

        /* Bildschirm zeichnen */
        draw_setup_card(username, password, password2,
                        focus, error, error_msg, mx, my);

        /* ── Mausklick: Feld auswählen ────────────────────────── */
        if (clicked) {
            if (hit(mx, my, SF_X, SU_Y, SF_W, FIELD_H)) {
                set_focus(&focus, 0, 3, field_names);
                error = false;
            } else if (hit(mx, my, SF_X, SP_Y, SF_W, FIELD_H)) {
                set_focus(&focus, 1, 3, field_names);
                error = false;
            } else if (hit(mx, my, SF_X, SP2_Y, SF_W, FIELD_H)) {
                set_focus(&focus, 2, 3, field_names);
                error = false;
            } else if (hit(mx, my, SETUP_BTN_X, SB_Y, BTN_W, BTN_H)) {
                serial_puts("[Login] Button 'Erstellen' geklickt\n");
                char pw2_tmp[LOGIN_PASSWORD_MAX];
                strncpy(pw2_tmp, password2, sizeof(pw2_tmp) - 1);
                if (try_create_user(username, password, pw2_tmp,
                                    &error, error_msg, sizeof(error_msg)))
                    return;
                memset(password,  0, sizeof(password));
                memset(password2, 0, sizeof(password2));
                focus = 1;
            }
        }

        /* ── Tastatur-Eingabe ─────────────────────────────────── */
        kbd_event_t ev;
        if (kbd_poll(&ev) && !ev.released) {
            error = false;

            if (ev.scancode == KEY_TAB) {
                set_focus(&focus, focus + 1, 3, field_names);
            } else if (ev.scancode == KEY_UP) {
                set_focus(&focus, focus - 1, 3, field_names);
            } else if (ev.scancode == KEY_DOWN) {
                set_focus(&focus, focus + 1, 3, field_names);
            } else if (ev.scancode == KEY_ENTER) {
                serial_puts("[Login] Enter: Erstellen versuchen\n");
                if (try_create_user(username, password, password2,
                                    &error, error_msg, sizeof(error_msg)))
                    return;
                memset(password,  0, sizeof(password));
                memset(password2, 0, sizeof(password2));
                focus = 1;
            } else if (ev.scancode == KEY_BACKSPACE) {
                char *f = (focus == 0) ? username
                        : (focus == 1) ? password : password2;
                int len = (int)strlen(f);
                if (len > 0) f[len - 1] = '\0';
            } else if (ev.ascii >= 32 && ev.ascii < 127) {
                char *f   = (focus == 0) ? username
                          : (focus == 1) ? password : password2;
                int   max = (focus == 0) ? LOGIN_USERNAME_MAX : LOGIN_PASSWORD_MAX;
                int   len = (int)strlen(f);
                if (len < max - 1) { f[len] = ev.ascii; f[len + 1] = '\0'; }
            }
        }

        /* Watchdog füttern: 16 ms schlafen → max ~60 FPS */
        task_sleep(16);
    }
}

/* ── users_init ─────────────────────────────────────────────── */
void users_init(void)
{
    user_count = 0;
}

/* ── login_run ──────────────────────────────────────────────── */
login_result_t login_run(void)
{
    if (user_count == 0)
        first_run_setup();

    char username[LOGIN_USERNAME_MAX] = {0};
    char password[LOGIN_PASSWORD_MAX] = {0};
    int  focus  = 0;
    bool error  = false;
    char error_msg[64] = {0};
    uint8_t prev_btn = 0;

    const char *field_names[] = { "Benutzername", "Passwort" };

    serial_puts("[Login] Anmelde-Bildschirm aktiv\n");
    serial_puts("[Login] Feld ausgewaehlt: Benutzername\n");

    for (;;) {
        /* Mausposition + Klick-Flanke */
        mouse_event_t me;
        while (mouse_poll(&me));
        int mx = mouse_get_x();
        int my = mouse_get_y();
        uint8_t cur_btn = mouse_get_buttons();
        bool clicked = (cur_btn & MOUSE_BTN_LEFT) && !(prev_btn & MOUSE_BTN_LEFT);
        prev_btn = cur_btn;

        /* Bildschirm zeichnen */
        draw_background();
        draw_clock();
        draw_card(username, password, focus, error, error_msg, mx, my);
        ui_flip();

        /* ── Mausklick: Feld oder Button auswählen ─────────────── */
        if (clicked) {
            if (hit(mx, my, FIELD_X, USER_Y, FIELD_W, FIELD_H)) {
                set_focus(&focus, 0, 2, field_names);
                error = false;
            } else if (hit(mx, my, FIELD_X, PASS_Y, FIELD_W, FIELD_H)) {
                set_focus(&focus, 1, 2, field_names);
                error = false;
            } else if (hit(mx, my, LOGIN_BTN_X, BTN_Y, BTN_W, BTN_H)) {
                serial_puts("[Login] Button 'Anmelden' geklickt\n");
                goto do_login;
            }
        }

        /* ── Tastatur-Eingabe ─────────────────────────────────── */
        kbd_event_t ev;
        if (kbd_poll(&ev) && !ev.released) {
            error = false;

            if (ev.scancode == KEY_TAB) {
                set_focus(&focus, focus + 1, 2, field_names);
            } else if (ev.scancode == KEY_UP) {
                set_focus(&focus, 0, 2, field_names);
            } else if (ev.scancode == KEY_DOWN) {
                set_focus(&focus, 1, 2, field_names);
            } else if (ev.scancode == KEY_ENTER) {
                do_login:;
                serial_puts("[Login] Anmeldeversuch: ");
                serial_puts(username);
                serial_puts("\n");
                os_user_t found;
                if (check_login(username, password, &found)) {
                    serial_puts("[Login] Erfolgreich angemeldet!\n");
                    login_result_t res;
                    res.success = true;
                    res.user    = found;
                    return res;
                } else {
                    serial_puts("[Login] Fehlgeschlagen - falsches Passwort\n");
                    error = true;
                    strncpy(error_msg,
                            "Falscher Benutzername oder Passwort",
                            sizeof(error_msg) - 1);
                    memset(password, 0, sizeof(password));
                    focus = 0;
                    serial_puts("[Login] Feld ausgewaehlt: Benutzername\n");
                }
            } else if (ev.scancode == KEY_BACKSPACE) {
                char *field = (focus == 0) ? username : password;
                int len = (int)strlen(field);
                if (len > 0) field[len - 1] = '\0';
            } else if (ev.ascii >= 32 && ev.ascii < 127) {
                if (focus == 0) {
                    int len = (int)strlen(username);
                    if (len < LOGIN_USERNAME_MAX - 1) {
                        username[len] = ev.ascii; username[len+1] = '\0';
                    }
                } else {
                    int len = (int)strlen(password);
                    if (len < LOGIN_PASSWORD_MAX - 1) {
                        password[len] = ev.ascii; password[len+1] = '\0';
                    }
                }
            }
        }

        /* Watchdog füttern: 16 ms schlafen → max ~60 FPS */
        task_sleep(16);
    }
}