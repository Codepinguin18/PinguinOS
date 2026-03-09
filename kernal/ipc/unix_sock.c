/**
 * @file unix_sock.c
 * @brief Unix-Domain-Sockets für PinguinOS – Feature #80.
 *
 * Jeder Socket hat zwei Ring-Puffer (je UNIX_SOCK_BUF_SIZE Bytes):
 *   - Eingangs-Puffer:  Daten die gelesen werden können
 *   - Ausgangs-Puffer:  Daten die gesendet wurden (Peer liest daraus)
 *
 * Verbindungsmodell:
 *   unix_sock_create("name")   legt Socket an (LISTEN-Zustand)
 *   unix_sock_connect("name")  erzeugt ein verbundenes Paar
 *   Beide Seiten teilen sich die gleichen Puffer.
 */

#include "../include/unix_sock.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/sync.h"

/* ── Socket-Zustände ─────────────────────────────────────────── */
typedef enum {
    SOCK_FREI      = 0,
    SOCK_HÖRT      = 1,   /* Wartet auf Verbindung (Server) */
    SOCK_VERBUNDEN = 2,   /* Verbunden mit einem Peer        */
} sock_zustand_t;

/* ── Ringpuffer-Struktur ─────────────────────────────────────── */
typedef struct {
    uint8_t  daten[UNIX_SOCK_BUF_SIZE];
    uint32_t lesen;    /* Lesezeiger  */
    uint32_t schreiben; /* Schreibzeiger */
    uint32_t count;    /* Belegte Bytes */
    mutex_t  sperre;
    condvar_t nicht_leer;
    condvar_t nicht_voll;
} ringpuffer_t;

/* ── Socket-Kontrollblock ────────────────────────────────────── */
typedef struct {
    uint32_t      id;
    sock_zustand_t zustand;
    char          name[UNIX_SOCK_NAME_LEN];
    ringpuffer_t *rx;      /* Eingangs-Puffer (von Peer gesendete Daten) */
    ringpuffer_t *tx;      /* Ausgangs-Puffer (eigene gesendete Daten)   */
    uint32_t      peer_id; /* ID des verbundenen Sockets                  */
} unix_socket_t;

/* ── Socket-Tabelle ──────────────────────────────────────────── */
static unix_socket_t sockets[UNIX_SOCK_MAX];
static spinlock_t    tabellen_sperre = SPINLOCK_INIT;
static uint32_t      naechste_id    = 1;

/* ── Ringpuffer-Hilfsfunktionen ──────────────────────────────── */
static ringpuffer_t *puffer_allozieren(void)
{
    ringpuffer_t *p = (ringpuffer_t *)kzalloc(sizeof(ringpuffer_t));
    if (!p) return NULL;
    mutex_init(&p->sperre);
    condvar_init(&p->nicht_leer);
    condvar_init(&p->nicht_voll);
    return p;
}

static int puffer_schreiben(ringpuffer_t *p, const uint8_t *src, uint32_t len)
{
    mutex_lock(&p->sperre);
    while (p->count + len > UNIX_SOCK_BUF_SIZE)
        condvar_wait(&p->nicht_voll, &p->sperre);

    for (uint32_t i = 0; i < len; i++) {
        p->daten[p->schreiben] = src[i];
        p->schreiben = (p->schreiben + 1) % UNIX_SOCK_BUF_SIZE;
        p->count++;
    }
    condvar_signal(&p->nicht_leer);
    mutex_unlock(&p->sperre);
    return (int)len;
}

static int puffer_lesen(ringpuffer_t *p, uint8_t *dst, uint32_t maxlen)
{
    mutex_lock(&p->sperre);
    while (p->count == 0)
        condvar_wait(&p->nicht_leer, &p->sperre);

    uint32_t kopieren = p->count < maxlen ? p->count : maxlen;
    for (uint32_t i = 0; i < kopieren; i++) {
        dst[i]   = p->daten[p->lesen];
        p->lesen = (p->lesen + 1) % UNIX_SOCK_BUF_SIZE;
        p->count--;
    }
    condvar_signal(&p->nicht_voll);
    mutex_unlock(&p->sperre);
    return (int)kopieren;
}

/* ── Hilfsfunktion: freien Slot suchen ───────────────────────── */
static unix_socket_t *freier_slot(void)
{
    for (int i = 0; i < UNIX_SOCK_MAX; i++)
        if (sockets[i].zustand == SOCK_FREI) return &sockets[i];
    return NULL;
}

/* ── Socket nach Name suchen ─────────────────────────────────── */
static unix_socket_t *suche_name(const char *name)
{
    for (int i = 0; i < UNIX_SOCK_MAX; i++)
        if (sockets[i].zustand != SOCK_FREI &&
            strcmp(sockets[i].name, name) == 0) return &sockets[i];
    return NULL;
}

/* ── Öffentlich: unix_sock_init ──────────────────────────────── */
void unix_sock_init(void)
{
    memset(sockets, 0, sizeof(sockets));
    KINFO("Unix-Sockets: initialisiert (%u Slots)\n", UNIX_SOCK_MAX);
}

/* ── Öffentlich: unix_sock_create ────────────────────────────── */
int unix_sock_create(const char *name)
{
    uint32_t flags = spin_lock_irqsave(&tabellen_sperre);
    if (suche_name(name)) {
        spin_unlock_irqrestore(&tabellen_sperre, flags);
        return -1;   /* Name bereits belegt */
    }

    unix_socket_t *s = freier_slot();
    if (!s) { spin_unlock_irqrestore(&tabellen_sperre, flags); return -1; }

    s->rx = puffer_allozieren();
    s->tx = puffer_allozieren();
    if (!s->rx || !s->tx) {
        if (s->rx) kfree(s->rx);
        if (s->tx) kfree(s->tx);
        spin_unlock_irqrestore(&tabellen_sperre, flags);
        return -1;
    }

    s->id       = naechste_id++;
    s->zustand  = SOCK_HÖRT;
    s->peer_id  = 0;
    strncpy(s->name, name, UNIX_SOCK_NAME_LEN - 1);

    int id = (int)s->id;
    spin_unlock_irqrestore(&tabellen_sperre, flags);
    KINFO("Unix-Socket: erstellt '%s' (id=%u)\n", name, s->id);
    return id;
}

/* ── Öffentlich: unix_sock_connect ───────────────────────────── */
int unix_sock_connect(const char *name)
{
    uint32_t flags = spin_lock_irqsave(&tabellen_sperre);
    unix_socket_t *server = suche_name(name);
    if (!server || server->zustand != SOCK_HÖRT) {
        spin_unlock_irqrestore(&tabellen_sperre, flags);
        return -1;
    }

    unix_socket_t *client = freier_slot();
    if (!client) { spin_unlock_irqrestore(&tabellen_sperre, flags); return -1; }

    /* Client teilt Puffer mit Server (überkreuz) */
    client->id       = naechste_id++;
    client->zustand  = SOCK_VERBUNDEN;
    client->rx       = server->tx;   /* Client liest was Server schreibt */
    client->tx       = server->rx;   /* Client schreibt was Server liest */
    client->peer_id  = server->id;
    strncpy(client->name, name, UNIX_SOCK_NAME_LEN - 1);

    server->zustand = SOCK_VERBUNDEN;
    server->peer_id = client->id;

    int id = (int)client->id;
    spin_unlock_irqrestore(&tabellen_sperre, flags);
    KINFO("Unix-Socket: verbunden mit '%s' (client_id=%u)\n", name, client->id);
    return id;
}

/* ── Öffentlich: unix_sock_send ──────────────────────────────── */
int unix_sock_send(int id, const void *daten, uint32_t len)
{
    for (int i = 0; i < UNIX_SOCK_MAX; i++) {
        if (sockets[i].zustand != SOCK_FREI && (int)sockets[i].id == id)
            return puffer_schreiben(sockets[i].tx, (const uint8_t *)daten, len);
    }
    return -1;
}

/* ── Öffentlich: unix_sock_recv ──────────────────────────────── */
int unix_sock_recv(int id, void *buf, uint32_t maxlen)
{
    for (int i = 0; i < UNIX_SOCK_MAX; i++) {
        if (sockets[i].zustand != SOCK_FREI && (int)sockets[i].id == id)
            return puffer_lesen(sockets[i].rx, (uint8_t *)buf, maxlen);
    }
    return -1;
}

/* ── Öffentlich: unix_sock_close ─────────────────────────────── */
void unix_sock_close(int id)
{
    uint32_t flags = spin_lock_irqsave(&tabellen_sperre);
    for (int i = 0; i < UNIX_SOCK_MAX; i++) {
        unix_socket_t *s = &sockets[i];
        if (s->zustand == SOCK_FREI || (int)s->id != id) continue;
        /* Puffer nur freigeben wenn Server schließt (Eigentümer) */
        if (s->zustand == SOCK_HÖRT && s->rx) { kfree(s->rx); kfree(s->tx); }
        memset(s, 0, sizeof(*s));
        break;
    }
    spin_unlock_irqrestore(&tabellen_sperre, flags);
}

void unix_sock_dump(void)
{
    serial_puts("[UNIX-SOCKETS]\n");
    for (int i = 0; i < UNIX_SOCK_MAX; i++) {
        unix_socket_t *s = &sockets[i];
        if (s->zustand == SOCK_FREI) continue;
        char buf[80];
        const char *zust = s->zustand == SOCK_HÖRT ? "HÖRT" : "VERBUNDEN";
        snprintf(buf, sizeof(buf), "  id=%u '%s'  %s  peer=%u\n",
                 s->id, s->name, zust, s->peer_id);
        serial_puts(buf);
    }
}