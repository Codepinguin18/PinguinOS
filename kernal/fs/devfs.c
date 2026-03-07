/**
 * @file devfs.c
 * @brief /dev virtual device filesystem for PinguinOS.
 *
 * Feature #52 – DevFS
 *
 * Provides a dynamic /dev directory whose entries are registered by
 * device drivers at runtime.  Each device appears as a VFS node;
 * read/write calls are forwarded to the driver's callback.
 *
 * Standard devices created at boot:
 *   /dev/null   – discards writes, returns EOF on read
 *   /dev/zero   – infinite zero bytes on read
 *   /dev/random – reads from the entropy pool
 *   /dev/tty    – forwards to VGA + keyboard (serial)
 *   /dev/hd0    – raw access to IDE drive 0
 */

#include "../include/vfs.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"
#include "../include/entropy.h"
#include "../include/ide.h"

/* ── Device entry ────────────────────────────────────────────────── */
#define DEVFS_MAX_DEVICES  32

typedef int32_t (*dev_read_fn) (uint32_t offset, uint32_t size, uint8_t *buf);
typedef int32_t (*dev_write_fn)(uint32_t offset, uint32_t size, const uint8_t *buf);

typedef struct {
    char         name[32];
    uint8_t      type;        /* VFS_CHARDEV or VFS_BLOCKDEV */
    dev_read_fn  read;
    dev_write_fn write;
    bool         used;
} devfs_dev_t;

static devfs_dev_t dev_table[DEVFS_MAX_DEVICES];

/* ── /dev/null ───────────────────────────────────────────────────── */
static int32_t null_read(uint32_t o, uint32_t s, uint8_t *b)
{ (void)o;(void)s;(void)b; return 0; }
static int32_t null_write(uint32_t o, uint32_t s, const uint8_t *b)
{ (void)o;(void)b; return (int32_t)s; }

/* ── /dev/zero ───────────────────────────────────────────────────── */
static int32_t zero_read(uint32_t o, uint32_t s, uint8_t *b)
{ (void)o; memset(b, 0, s); return (int32_t)s; }

/* ── /dev/random ─────────────────────────────────────────────────── */
static int32_t random_read(uint32_t o, uint32_t s, uint8_t *b)
{ (void)o; rand_bytes(b, s); return (int32_t)s; }

/* ── /dev/tty (serial output) ────────────────────────────────────── */
static int32_t tty_write(uint32_t o, uint32_t s, const uint8_t *b)
{
    (void)o;
    for (uint32_t i = 0; i < s; i++) serial_putc((char)b[i]);
    return (int32_t)s;
}
static int32_t tty_read(uint32_t o, uint32_t s, uint8_t *b)
{ (void)o;(void)s;(void)b; return 0; }

/* ── /dev/hd0 (raw IDE) ──────────────────────────────────────────── */
static int32_t hd0_read(uint32_t offset, uint32_t size, uint8_t *buf)
{
    uint32_t lba   = offset / 512;
    uint32_t count = (size + 511) / 512;
    if (count > 127) count = 127;
    uint8_t tmp[127 * 512];
    if (ide_read(0, lba, (uint8_t)count, tmp) != 0) return -1;
    uint32_t copy = count * 512;
    if (copy > size) copy = size;
    memcpy(buf, tmp, copy);
    return (int32_t)copy;
}
static int32_t hd0_write(uint32_t offset, uint32_t size, const uint8_t *buf)
{
    uint32_t lba   = offset / 512;
    uint32_t count = (size + 511) / 512;
    if (count > 127) count = 127;
    uint8_t tmp[127 * 512];
    memcpy(tmp, buf, count * 512);
    if (ide_write(0, lba, (uint8_t)count, tmp) != 0) return -1;
    return (int32_t)(count * 512);
}

/* ── VFS operations ──────────────────────────────────────────────── */
static int32_t devfs_read(vfs_node_t *node, uint32_t offset,
                           uint32_t size, uint8_t *buf)
{
    devfs_dev_t *d = (devfs_dev_t *)node->fs_data;
    if (!d || !d->read) return -1;
    return d->read(offset, size, buf);
}

static int32_t devfs_write(vfs_node_t *node, uint32_t offset,
                            uint32_t size, const uint8_t *buf)
{
    devfs_dev_t *d = (devfs_dev_t *)node->fs_data;
    if (!d || !d->write) return -1;
    return d->write(offset, size, buf);
}

static bool devfs_readdir(vfs_node_t *node, uint32_t index,
                           vfs_dirent_t *out)
{
    (void)node;
    uint32_t count = 0;
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        if (!dev_table[i].used) continue;
        if (count == index) {
            strncpy(out->name, dev_table[i].name, 255);
            out->inode_num = (uint32_t)(200 + i);
            out->type      = dev_table[i].type;
            return true;
        }
        count++;
    }
    return false;
}

static vfs_node_t *devfs_finddir(vfs_node_t *node, const char *name)
{
    (void)node;
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        if (!dev_table[i].used) continue;
        if (strncmp(dev_table[i].name, name, 31) != 0) continue;

        vfs_node_t *n = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
        if (!n) return NULL;
        strncpy(n->name, name, 255);
        n->flags   = dev_table[i].type;
        n->ops     = &devfs_ops;
        n->fs_data = &dev_table[i];
        return n;
    }
    return NULL;
}

static vfs_node_t *devfs_mount(const char *device, uint32_t flags)
{
    (void)device; (void)flags;
    vfs_node_t *root = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!root) return NULL;
    memcpy(root->name, "dev", 4);
    root->flags = VFS_DIR;
    root->ops   = &devfs_ops;
    return root;
}

/* ── Public: devfs_register ──────────────────────────────────────── */
int devfs_register(const char *name, uint8_t type,
                   dev_read_fn read_fn, dev_write_fn write_fn)
{
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        if (dev_table[i].used) continue;
        strncpy(dev_table[i].name, name, 31);
        dev_table[i].type  = type;
        dev_table[i].read  = read_fn;
        dev_table[i].write = write_fn;
        dev_table[i].used  = true;
        KINFO("devfs: registered /dev/%s\n", name);
        return 0;
    }
    return -1;
}

/* ── Public: devfs_init ──────────────────────────────────────────── */
void devfs_init(void)
{
    memset(dev_table, 0, sizeof(dev_table));

    devfs_register("null",   VFS_CHARDEV,  null_read,   null_write);
    devfs_register("zero",   VFS_CHARDEV,  zero_read,   null_write);
    devfs_register("random", VFS_CHARDEV,  random_read, NULL);
    devfs_register("tty",    VFS_CHARDEV,  tty_read,    tty_write);
    devfs_register("hd0",    VFS_BLOCKDEV, hd0_read,    hd0_write);

    KINFO("devfs: /dev filesystem initialised (%d devices)\n", 5);
}

/* ── Public fs_ops ───────────────────────────────────────────────── */
fs_ops_t devfs_ops = {
    .name    = "devfs",
    .read    = devfs_read,
    .write   = devfs_write,
    .open    = NULL,
    .close   = NULL,
    .readdir = devfs_readdir,
    .finddir = devfs_finddir,
    .mkdir   = NULL,
    .unlink  = NULL,
    .mount   = devfs_mount,
    .unmount = NULL,
};