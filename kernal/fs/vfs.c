/**
 * @file vfs.c
 * @brief VFS-Kern-Layer für PinguinOS.
 */
#include "../include/vfs.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/cpu.h"

/* ── Globaler Zustand ────────────────────────────────────────────── */
static vfs_mount_t mount_table[VFS_MAX_MOUNTS];
static fs_ops_t   *fs_registry[16];
static uint32_t    fs_count = 0;

/* ── vfs_init ────────────────────────────────────────────────────── */
void vfs_init(void)
{
    memset(mount_table, 0, sizeof(mount_table));
    memset(fs_registry, 0, sizeof(fs_registry));
    fs_count = 0;
    KINFO("VFS: initialisiert\n");
}

/* ── vfs_register_fs ─────────────────────────────────────────────── */
void vfs_register_fs(fs_ops_t *ops)
{
    if (fs_count < 16) {
        fs_registry[fs_count++] = ops;
        KINFO("VFS: Dateisystem '%s' registriert\n", ops->name);
    }
}

/* ── vfs_mount ───────────────────────────────────────────────────── */
int vfs_mount(const char *path, const char *device,
              const char *fsname, uint32_t flags)
{
    /* Freien Mount-Slot suchen */
    vfs_mount_t *slot = NULL;
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        if (!mount_table[i].used) { slot = &mount_table[i]; break; }
    }
    if (!slot) return -1;

    /* Passendes FS finden */
    fs_ops_t *ops = NULL;
    for (uint32_t i = 0; i < fs_count; i++) {
        if (strcmp(fs_registry[i]->name, fsname) == 0) {
            ops = fs_registry[i];
            break;
        }
    }
    if (!ops || !ops->mount) return -2;

    vfs_node_t *root = ops->mount(device, flags);
    if (!root) return -3;

    strncpy(slot->path, path, VFS_PATH_MAX - 1);
    slot->root = root;
    slot->ops  = ops;
    slot->used = true;

    KINFO("VFS: '%s' als '%s' an '%s' gemountet\n", device, fsname, path);
    return 0;
}

/* ── vfs_lookup ──────────────────────────────────────────────────── */
vfs_node_t *vfs_lookup(const char *path)
{
    /* Längsten passenden Mount-Punkt finden */
    vfs_mount_t *best = NULL;
    size_t       best_len = 0;

    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        if (!mount_table[i].used) continue;
        size_t plen = strlen(mount_table[i].path);
        if (strncmp(mount_table[i].path, path, plen) == 0 &&
            plen > best_len) {
            best     = &mount_table[i];
            best_len = plen;
        }
    }
    if (!best) return NULL;

    /* Pfad-Komponenten vom Wurzelknoten aus traversieren */
    vfs_node_t *node = best->root;
    const char *rel  = path + best_len;

    /* Führende Slashes überspringen */
    while (*rel == '/') rel++;

    while (*rel && node) {
        char component[256];
        const char *slash = strchr(rel, '/');
        size_t len = slash ? (size_t)(slash - rel) : strlen(rel);
        if (len >= sizeof(component)) return NULL;

        strncpy(component, rel, len);
        component[len] = '\0';

        if (node->ops && node->ops->finddir)
            node = node->ops->finddir(node, component);
        else
            return NULL;

        rel += len;
        while (*rel == '/') rel++;
    }

    return node;
}

vfs_node_t *vfs_open(const char *path, uint32_t flags)
{
    vfs_node_t *node = vfs_lookup(path);
    if (!node && (flags & O_CREAT)) {
        /* Datei anlegen – Elternverzeichnis finden */
        /* (Vereinfacht: nur in rootfs) */
    }
    if (node) node->ref_count++;
    return node;
}

void vfs_close(vfs_node_t *node)
{
    if (!node) return;
    if (node->ref_count > 0) node->ref_count--;
    if (node->ops && node->ops->close) node->ops->close(node);
}

int32_t vfs_read(vfs_node_t *node, uint32_t offset,
                 uint32_t size, uint8_t *buf)
{
    if (!node || !node->ops || !node->ops->read) return -1;
    return node->ops->read(node, offset, size, buf);
}

int32_t vfs_write(vfs_node_t *node, uint32_t offset,
                  uint32_t size, const uint8_t *buf)
{
    if (!node || !node->ops || !node->ops->write) return -1;
    return node->ops->write(node, offset, size, buf);
}

bool vfs_readdir(vfs_node_t *node, uint32_t idx, vfs_dirent_t *out)
{
    if (!node || !node->ops || !node->ops->readdir) return false;
    return node->ops->readdir(node, idx, out);
}

vfs_node_t *vfs_finddir(vfs_node_t *node, const char *name)
{
    if (!node || !node->ops || !node->ops->finddir) return NULL;
    return node->ops->finddir(node, name);
}

int vfs_umount(const char *path)
{
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        if (mount_table[i].used && strcmp(mount_table[i].path, path) == 0) {
            mount_table[i].used = false;
            return 0;
        }
    }
    return -1;
}

void vfs_dump(void)
{
    KINFO("VFS Mount-Tabelle:\n");
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        if (!mount_table[i].used) continue;
        KINFO("  %-20s  FS=%-10s\n",
              mount_table[i].path,
              mount_table[i].ops ? mount_table[i].ops->name : "?");
    }
}
