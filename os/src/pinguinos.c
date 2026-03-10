/**
 * @file pinguinos.c
 * @brief PinguinOS Hauptschleife – Login → Desktop → Logout → Login.
 *
 * Wird vom Kernel (kernel.c) nach vollständiger Hardware-Initialisierung
 * über pinguinos_main() aufgerufen.
 */

#include "../include/pinguinos.h"
#include "../include/ui.h"
#include "../include/login.h"
#include "../include/desktop.h"
#include "../../kernal/include/klib.h"
#include "../../kernal/include/serial.h"
#include "../../kernal/include/bga.h"

/* ── Abschluss-Bildschirm (Herunterfahren) ───────────────────── */
static void shutdown_screen(void)
{
    ui_clear(COL_BG);
    ui_fill_rect(0, 0, SCREEN_W, 4, COL_ACCENT);

    ui_draw_text_centered(0, SCREEN_H / 2 - 20, SCREEN_W, 16,
                           "PinguinOS wird heruntergefahren...",
                           COL_ACCENT);
    ui_draw_text_centered(0, SCREEN_H / 2 + 4, SCREEN_W, 12,
                           "Strom kann jetzt getrennt werden.",
                           COL_GRAY);

    /* Teal-Fortschrittsbalken */
    int bw = 300, bh = 8;
    int bx = (SCREEN_W - bw) / 2;
    int by = SCREEN_H / 2 + 30;
    ui_fill_rect(bx, by, bw, bh, COL_DARK_GRAY);
    ui_fill_rect(bx, by, bw, bh, COL_ACCENT);
    ui_draw_rect(bx - 1, by - 1, bw + 2, bh + 2, COL_ACCENT_DIM);

    ui_flip();
}

/* ── Öffentlich: pinguinos_main ─────────────────────────────── */
void pinguinos_main(void)
{
    serial_puts("[PinguinOS] Starte Desktop-Umgebung\n");

    /* Grafikmodus initialisieren */
    ui_init();

    /* Benutzerverwaltung initialisieren */
    users_init();

    /* Haupt-Schleife: Login → Desktop → (Logout → Login | Shutdown) */
    for (;;) {
        /* Login-Bildschirm */
        serial_puts("[PinguinOS] Login-Bildschirm\n");
        login_result_t result = login_run();

        if (!result.success) continue;   /* Sollte nie eintreten */

        serial_puts("[PinguinOS] Angemeldet als: ");
        serial_puts(result.user.username);
        serial_puts("\n");

        /* Desktop ausführen */
        desktop_state_t state = desktop_run(&result.user);

        if (state == DESKTOP_SHUTDOWN) {
            shutdown_screen();
            /* CPU in Haltezustand versetzen */
            __asm__ volatile ("cli; hlt");
            for (;;);   /* Sollte nie erreicht werden */
        }
        /* Bei LOGOUT: zurück zum Login */
        serial_puts("[PinguinOS] Abgemeldet, zuruck zum Login\n");
    }
}