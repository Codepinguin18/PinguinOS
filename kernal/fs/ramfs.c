/**
 * @file ramfs.c
 * @brief RAMFS – temporäres Dateisystem im Arbeitsspeicher (Feature #47).
 *
 * RAMFS hält alle Dateien als verkettete Liste von ramfs_node_t im Heap.
 * Ideal für /tmp, /dev, oder als initramfs-Ersatz.
 */
#include "../include/vfs.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Interne RAMFS-Knoten-Struktur ──────────────────────────────── */
#define RAMFS_BLOCK_SIZE  4096
#define RAMFS_MAX_SIZE    (16 * 1024 * 1024)   /* 16 MB gesamt */

typedef struct ramfs_node {
    vfs_node_t       vfs;          /* Muss erster Member sein!         */
    uint8_t         *data;         /* Datei-Inhalt (NULL für Verz.)    */
    uint32_t         alloc_size;   /* Tatsächlich allozierte Bytes     */
    struct ramfs_node *children;   /* Verzeichnis-Kindknoten (Liste)   */
    struct ramfs_node *siblings;   /* Geschwister-Knoten               */
    struct ramfs_node *parent;
} ramfs_node_t;

static uint32_t ramfs_inode_counter = 1;

/* ── Neuen RAMFS-Knoten erstellen ───────────────────────────────── */
static ramfs_node_t *ramfs_new_node(const char *name, uint32_t flags)
{
    ramfs_node_t *n = (ramfs_node_t *)kzalloc(sizeof(ramfs_node_t));
    if (!n) return NULL;

    strncpy(n->vfs.name, name, sizeof(n->vfs.name) - 1);
    n->vfs.inode_num = ramfs_inode_counter++;
    n->vfs.flags     = flags;
    n->vfs.ops       = &ramfs_ops;
    n->vfs.fs_data   = n;
    return n;
}

/* ── Implementierung der FS-Ops ──────────────────────────────────── */
static int32_t ramfs_read(vfs_node_t *node, uint32_t offset,
                           uint32_t size, uint8_t *buf)
{
    ramfs_node_t *rn = (ramfs_node_t *)node->fs_data;
    if (!rn->data || offset >= node->size) return 0;

    uint32_t avail = node->size - offset;
    uint32_t to_read = (size < avail) ? size : avail;
    memcpy(buf, rn->data + offset, to_read);
    return (int32_t)to_read;
}

static int32_t ramfs_write(vfs_node_t *node, uint32_t offset,
                            uint32_t size, const uint8_t *buf)
{
    ramfs_node_t *rn = (ramfs_node_t *)node->fs_data;

    uint32_t needed = offset + size;
    if (needed > RAMFS_MAX_SIZE) return -1;

    /* Puffer vergrößern falls nötig */
    if (needed > rn->alloc_size) {
        uint32_t new_size = ALIGN_UP(needed, RAMFS_BLOCK_SIZE);
        uint8_t *new_data = (uint8_t *)krealloc(rn->data, new_size);
        if (!new_data) return -1;
        rn->data        = new_data;
        rn->alloc_size  = new_size;
    }

    memcpy(rn->data + offset, buf, size);
    if (offset + size > node->size) node->size = offset + size;
    return (int32_t)size;
}

static bool ramfs_readdir(vfs_node_t *node, uint32_t index,
                           vfs_dirent_t *out)
{
    ramfs_node_t *rn  = (ramfs_node_t *)node->fs_data;
    ramfs_node_t *cur = rn->children;

    /* Spezial-Einträge: . und .. */
    if (index == 0) {
        strcpy(out->name, ".");
        out->inode_num = node->inode_num;
        out->type      = VFS_DIR;
        return true;
    }
    if (index == 1) {
        strcpy(out->name, "..");
        out->inode_num = rn->parent ? rn->parent->vfs.inode_num : 1;
        out->type      = VFS_DIR;
        return true;
    }
    index -= 2;

    while (cur && index--) cur = cur->siblings;
    if (!cur) return false;

    strncpy(out->name, cur->vfs.name, 255);
    out->inode_num = cur->vfs.inode_num;
    out->type      = (uint8_t)(cur->vfs.flags & 0xFF);
    return true;
}

static vfs_node_t *ramfs_finddir(vfs_node_t *node, const char *name)
{
    ramfs_node_t *rn = (ramfs_node_t *)node->fs_data;
    ramfs_node_t *cur = rn->children;
    while (cur) {
        if (strcmp(cur->vfs.name, name) == 0) return &cur->vfs;
        cur = cur->siblings;
    }
    return NULL;
}

static int ramfs_mkdir(vfs_node_t *parent, const char *name, uint32_t flags)
{
    UNUSED(flags);
    ramfs_node_t *prn = (ramfs_node_t *)parent->fs_data;
    ramfs_node_t *dir = ramfs_new_node(name, VFS_DIR);
    if (!dir) return -1;
    dir->parent       = prn;
    dir->siblings     = prn->children;
    prn->children     = dir;
    return 0;
}

static int ramfs_unlink(vfs_node_t *parent, const char *name)
{
    ramfs_node_t *prn = (ramfs_node_t *)parent->fs_data;
    ramfs_node_t **pp = &prn->children;
    while (*pp) {
        if (strcmp((*pp)->vfs.name, name) == 0) {
            ramfs_node_t *del = *pp;
            *pp = del->siblings;
            kfree(del->data);
            kfree(del);
            return 0;
        }
        pp = &(*pp)->siblings;
    }
    return -1;
}

static vfs_node_t *ramfs_mount(const char *device, uint32_t flags)
{
    UNUSED(device); UNUSED(flags);
    ramfs_node_t *root = ramfs_new_node("/", VFS_DIR);
    if (!root) return NULL;
    KINFO("RAMFS: gemountet\n");
    return &root->vfs;
}

/* ── FS-Ops-Tabelle ──────────────────────────────────────────────── */
fs_ops_t ramfs_ops = {
    .name    = "ramfs",
    .read    = ramfs_read,
    .write   = ramfs_write,
    .open    = NULL,
    .close   = NULL,
    .readdir = ramfs_readdir,
    .finddir = ramfs_finddir,
    .mkdir   = ramfs_mkdir,
    .unlink  = ramfs_unlink,
    .mount   = ramfs_mount,
    .unmount = NULL,
};
