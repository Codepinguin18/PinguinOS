/**
 * @file login.h
 * @brief PinguinOS Login-Bildschirm.
 */
#ifndef _OS_LOGIN_H
#define _OS_LOGIN_H

#include "../../kernal/include/types.h"

#define LOGIN_USERNAME_MAX  32
#define LOGIN_PASSWORD_MAX  64
#define LOGIN_MAX_USERS     8

/* ── Benutzereintrag ─────────────────────────────────────────── */
typedef struct {
    char username[LOGIN_USERNAME_MAX];
    char password_hash[65];   /* SHA-256 als Hex-String (simplifiziert: Klartext) */
    bool admin;
} os_user_t;

/* ── Login-Ergebnis ──────────────────────────────────────────── */
typedef struct {
    bool      success;
    os_user_t user;
} login_result_t;

/* ── Öffentliche API ─────────────────────────────────────────── */

/** Benutzerverwaltung initialisieren (Standard-Benutzer anlegen). */
void users_init(void);

/** Benutzer hinzufügen. */
bool users_add(const char *username, const char *password, bool admin);

/** Login-Bildschirm anzeigen und auf Eingabe warten.
 *  Blockiert bis Login erfolgreich.
 *  @return Angemeldeter Benutzer.
 */
login_result_t login_run(void);

#endif /* _OS_LOGIN_H */