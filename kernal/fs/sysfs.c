/**
 * @file sysfs.c
 * @brief SysFS – virtuelles Gerätedateisystem für PinguinOS – Feature #54.
 *
 * Eingebundene Standardeinträge (nach sysfs_init):
 *   /sys/version      – Kernel-Version "2.3"
 *   /sys/uptime       – Laufzeit in Millisekunden
 *   /sys/meminfo      – freier physischer Speicher
 *   /sys/scheduler    – Scheduler-Informationen
 *   /sys/cpuid        – CPU-Modellname
 *
 * Treiber können weitere Einträge mit sysfs_register() hinzufügen.
 * Beispiel (in watchdog.c):
 *   sysfs_register("watchdog/timeout", wd_read, wd_write);
 */

#include "../include/sysfs.h"
#include "../include/vfs.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/scheduler.h"
#include "../include/mm.h"
#include "../include/cpu.h"

/* ── Interne Eintrags-Tabelle ─────────────────────────────────── */
typedef struct {
    char         name[SYSFS_NAME_LEN];
    sysfs_read_fn  rd;
    sysfs_write_fn wr;
    bool         belegt;
} sysfs_entry_t;

static sysfs_entry_t eintraege[SYSFS_ENTRIES_MAX];

/* ── Standard-Lese-Callbacks ─────────────────────────────────── */
static int sysfs_rd_version(char *buf, uint32_t max)
{ return snprintf(buf, max, "PinguinOS 2.3\n"); }

static int sysfs_rd_uptime(char *buf, uint32_t max)
{ return snprintf(buf, max, "%u\n", sched_uptime_ms()); }

static int sysfs_rd_meminfo(char *buf, uint32_t max)
{
    uint32_t frei = pmm_free_page_count() * PAGE_SIZE / 1024;
    return snprintf(buf, max, "FreeKB: %u\n", frei);
}

static int sysfs_rd_scheduler(char *buf, uint32_t max)
{
    return snprintf(buf, max, "Tasks: %u  Laufzeit: %u ms\n",
                   sched_task_count(), sched_uptime_ms());
}

static int sysfs_rd_cpuid(char *buf, uint32_t max)
{
    const cpu_info_t *ci = cpu_get_info();
    return snprintf(buf, max, "%s\n", ci->brand);
}

/* ── Öffentlich: sysfs_register ─────────────────────────────── */
int sysfs_register(const char *name, sysfs_read_fn rd, sysfs_write_fn wr)
{
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++) {
        if (!eintraege[i].belegt) {
            strncpy(eintraege[i].name, name, SYSFS_NAME_LEN - 1);
            eintraege[i].rd     = rd;
            eintraege[i].wr     = wr;
            eintraege[i].belegt = true;
            return 0;
        }
    }
    return -1;
}

void sysfs_unregister(const char *name)
{
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++)
        if (eintraege[i].belegt && strcmp(eintraege[i].name, name) == 0)
            { eintraege[i].belegt = false; return; }
}

/* ── VFS-Lesen ───────────────────────────────────────────────── */
static int32_t sysfs_read(vfs_node_t *node, uint32_t offset,
                           uint32_t size, uint8_t *buf)
{
    /* Name des Knotens als Schlüssel suchen */
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++) {
        if (!eintraege[i].belegt) continue;
        if (strcmp(eintraege[i].name, node->name) != 0) continue;
        if (!eintraege[i].rd) return 0;

        /* Wert in temporären Puffer lesen */
        char tmp[SYSFS_VAL_LEN];
        int len = eintraege[i].rd(tmp, sizeof(tmp));
        if (len <= 0) return 0;
        if (offset >= (uint32_t)len) return 0;

        uint32_t kopieren = (uint32_t)len - offset;
        if (kopieren > size) kopieren = size;
        memcpy(buf, tmp + offset, kopieren);
        return (int32_t)kopieren;
    }
    return -1;
}

/* ── VFS-Schreiben ───────────────────────────────────────────── */
static int32_t sysfs_write(vfs_node_t *node, uint32_t offset,
                            uint32_t size, const uint8_t *buf)
{
    (void)offset;
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++) {
        if (!eintraege[i].belegt) continue;
        if (strcmp(eintraege[i].name, node->name) != 0) continue;
        if (!eintraege[i].wr) return -1;
        return eintraege[i].wr((const char *)buf, size) == 0 ? (int32_t)size : -1;
    }
    return -1;
}

/* ── VFS-Verzeichnislesung ───────────────────────────────────── */
static bool sysfs_readdir(vfs_node_t *node, uint32_t index, vfs_dirent_t *out)
{
    (void)node;
    uint32_t gesehen = 0;
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++) {
        if (!eintraege[i].belegt) continue;
        if (gesehen == index) {
            strncpy(out->name, eintraege[i].name, 255);
            out->type = VFS_FILE;
            return true;
        }
        gesehen++;
    }
    return false;
}

/* ── VFS-Suche ───────────────────────────────────────────────── */
static vfs_node_t *sysfs_finddir(vfs_node_t *parent, const char *name)
{
    (void)parent;
    for (int i = 0; i < SYSFS_ENTRIES_MAX; i++) {
        if (!eintraege[i].belegt) continue;
        if (strcmp(eintraege[i].name, name) != 0) continue;

        vfs_node_t *n = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
        if (!n) return NULL;
        strncpy(n->name, eintraege[i].name, 255);
        n->flags = VFS_FILE;
        n->size  = SYSFS_VAL_LEN;
        n->ops   = &sysfs_ops;
        return n;
    }
    return NULL;
}

/* ── VFS-Einhängen ───────────────────────────────────────────── */
static vfs_node_t *sysfs_mount(const char *device, uint32_t flags)
{
    (void)device; (void)flags;

    /* Standardeinträge registrieren */
    memset(eintraege, 0, sizeof(eintraege));
    sysfs_register("version",   sysfs_rd_version,   NULL);
    sysfs_register("uptime",    sysfs_rd_uptime,     NULL);
    sysfs_register("meminfo",   sysfs_rd_meminfo,    NULL);
    sysfs_register("scheduler", sysfs_rd_scheduler,  NULL);
    sysfs_register("cpuid",     sysfs_rd_cpuid,      NULL);

    vfs_node_t *wurzel = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!wurzel) return NULL;
    memcpy(wurzel->name, "/", 2);
    wurzel->flags = VFS_DIR;
    wurzel->ops   = &sysfs_ops;

    KINFO("SysFS: eingehängt mit %u Standard-Einträgen\n", 5);
    return wurzel;
}

/* ── Öffentliche fs_ops-Struktur ─────────────────────────────── */
fs_ops_t sysfs_ops = {
    .name    = "sysfs",
    .read    = sysfs_read,
    .write   = sysfs_write,
    .readdir = sysfs_readdir,
    .finddir = sysfs_finddir,
    .mount   = sysfs_mount,
};