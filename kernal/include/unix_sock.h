/**
 * @file unix_sock.h
 * @brief Unix-Domain-Sockets für PinguinOS – Feature #80.
 *
 * Bietet lokale IPC über benannte Sockets (ähnlich POSIX AF_UNIX).
 * Sockets werden im Namespace /dev/sock/ verwaltet.
 *
 * API (vereinfacht, kernelseitig):
 *   unix_sock_create(name)   – benannten Socket erstellen
 *   unix_sock_connect(name)  – zu bestehendem Socket verbinden
 *   unix_sock_send(id, ...)  – Daten senden
 *   unix_sock_recv(id, ...)  – Daten empfangen (blockiert)
 *   unix_sock_close(id)      – Socket schließen
 */
#ifndef _UNIX_SOCK_H
#define _UNIX_SOCK_H

#include "types.h"

#define UNIX_SOCK_MAX       16      /* Maximale gleichzeitige Sockets    */
#define UNIX_SOCK_NAME_LEN  64      /* Maximale Pfadlänge                */
#define UNIX_SOCK_BUF_SIZE  4096    /* Puffergröße pro Socket            */

/** Unix-Socket-Subsystem initialisieren. */
void unix_sock_init(void);

/** Benannten Server-Socket erstellen. Gibt ID zurück, -1 bei Fehler. */
int  unix_sock_create(const char *name);

/** Zu einem benannten Socket verbinden. Gibt ID zurück, -1 bei Fehler. */
int  unix_sock_connect(const char *name);

/** Daten über einen Socket senden. */
int  unix_sock_send(int id, const void *daten, uint32_t len);

/** Daten von einem Socket empfangen (blockiert bis Daten verfügbar). */
int  unix_sock_recv(int id, void *buf, uint32_t maxlen);

/** Socket schließen. */
void unix_sock_close(int id);

/** Alle Sockets auf seriell ausgeben. */
void unix_sock_dump(void);

#endif /* _UNIX_SOCK_H */