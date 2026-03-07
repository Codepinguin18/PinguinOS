/**
 * @file procfs.c
 * @brief /proc virtual filesystem for PinguinOS.
 *
 * Feature #53 – ProcFS
 *
 * Exposes kernel state as read-only virtual files under /proc:
 *
 *   /proc/uptime      – system uptime in milliseconds
 *   /proc/meminfo     – heap usage statistics
 *   /proc/cpuinfo     – CPU vendor, brand, features
 *   /proc/net/dev     – network interface table
 *   /proc/<pid>/      – per-process directory (name, state, pid)
 *   /proc/<pid>/stat  – process accounting
 *
 * Each "file" is generated on read by a callback function.
 * No data is stored persistently; every read re-generates the content.
 */

#include "../include/vfs.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/cpu.h"
#include "../include/heap.h"
#include "../include/net.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Generator function type ─────────────────────────────────────── */
typedef uint32_t (*procfs_gen_fn)(uint8_t *buf, uint32_t size,
                                   const void *arg);

/* ── Static file descriptor ──────────────────────────────────────── */
typedef struct {
    const char   *name;
    procfs_gen_fn gen;
    const void   *arg;
} procfs_file_t;

/* ── Generators ──────────────────────────────────────────────────── */
static uint32_t gen_uptime(uint8_t *buf, uint32_t size, const void *arg)
{
    (void)arg;
    uint32_t ms = sched_uptime_ms();
    return (uint32_t)snprintf((char *)buf, size,
        "%u.%03u\n", ms / 1000, ms % 1000);
}

static uint32_t gen_meminfo(uint8_t *buf, uint32_t size, const void *arg)
{
    (void)arg;
    extern size_t heap_used(void);
    extern size_t heap_available(void);
    size_t used = heap_used();
    size_t avail = heap_available();
    return (uint32_t)snprintf((char *)buf, size,
        "HeapUsed:      %8u kB\n"
        "HeapFree:      %8u kB\n"
        "HeapTotal:     %8u kB\n",
        (uint32_t)(used  / 1024),
        (uint32_t)(avail / 1024),
        (uint32_t)((used + avail) / 1024));
}

static uint32_t gen_cpuinfo(uint8_t *buf, uint32_t size, const void *arg)
{
    (void)arg;
    const cpu_info_t *ci = cpu_get_info();
    return (uint32_t)snprintf((char *)buf, size,
        "vendor_id  : %s\n"
        "model name : %s\n"
        "cpu family : %u\n"
        "model      : %u\n"
        "stepping   : %u\n"
        "flags_edx  : 0x%08x\n"
        "flags_ecx  : 0x%08x\n",
        ci->vendor, ci->brand,
        ci->family, ci->model, ci->stepping,
        ci->features_edx, ci->features_ecx);
}

static uint32_t gen_netdev(uint8_t *buf, uint32_t size, const void *arg)
{
    (void)arg;
    const net_stats_t *st = net_get_stats();
    return (uint32_t)snprintf((char *)buf, size,
        "%-8s  RX-pkts: %u  RX-bytes: %u  TX-pkts: %u  TX-bytes: %u\n",
        "all",
        (uint32_t)st->rx_packets, (uint32_t)st->rx_bytes,
        (uint32_t)st->tx_packets, (uint32_t)st->tx_bytes);
}

static uint32_t gen_tasks(uint8_t *buf, uint32_t size, const void *arg)
{
    (void)arg;
    uint32_t n = sched_task_count();
    return (uint32_t)snprintf((char *)buf, size,
        "tasks_active: %u\n"
        "uptime_ms:    %u\n",
        n, sched_uptime_ms());
}

/* ── Static file table ───────────────────────────────────────────── */
static procfs_file_t procfs_files[] = {
    { "uptime",   gen_uptime,  NULL },
    { "meminfo",  gen_meminfo, NULL },
    { "cpuinfo",  gen_cpuinfo, NULL },
    { "net/dev",  gen_netdev,  NULL },
    { "tasks",    gen_tasks,   NULL },
};
#define PROCFS_NUM_FILES  (sizeof(procfs_files) / sizeof(procfs_files[0]))

/* ── VFS operations ──────────────────────────────────────────────── */
static int32_t procfs_read(vfs_node_t *node, uint32_t offset,
                            uint32_t size, uint8_t *buf)
{
    if (!node->fs_data) return -1;
    procfs_file_t *f = (procfs_file_t *)node->fs_data;

    /* Generate content into a temporary buffer */
    uint8_t tmp[2048];
    uint32_t total = f->gen(tmp, sizeof(tmp), f->arg);

    if (offset >= total) return 0;
    uint32_t avail = total - offset;
    uint32_t n     = (size < avail) ? size : avail;
    memcpy(buf, tmp + offset, n);
    return (int32_t)n;
}

static bool procfs_readdir(vfs_node_t *node, uint32_t index,
                            vfs_dirent_t *out)
{
    (void)node;
    if (index >= PROCFS_NUM_FILES) return false;
    strncpy(out->name, procfs_files[index].name, 255);
    out->inode_num = (uint32_t)(100 + index);
    out->type      = VFS_FILE;
    return true;
}

static vfs_node_t *procfs_finddir(vfs_node_t *node, const char *name)
{
    (void)node;
    for (uint32_t i = 0; i < PROCFS_NUM_FILES; i++) {
        if (strcmp(procfs_files[i].name, name) == 0) {
            vfs_node_t *n = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
            if (!n) return NULL;
            strncpy(n->name, name, 255);
            n->flags   = VFS_FILE;
            n->ops     = &procfs_ops;
            n->fs_data = &procfs_files[i];
            n->size    = 4096;
            return n;
        }
    }
    return NULL;
}

static vfs_node_t *procfs_mount(const char *device, uint32_t flags)
{
    (void)device; (void)flags;
    vfs_node_t *root = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!root) return NULL;
    memcpy(root->name, "proc", 5);
    root->flags = VFS_DIR;
    root->ops   = &procfs_ops;
    return root;
}

/* ── Public fs_ops ───────────────────────────────────────────────── */
fs_ops_t procfs_ops = {
    .name    = "procfs",
    .read    = procfs_read,
    .write   = NULL,
    .open    = NULL,
    .close   = NULL,
    .readdir = procfs_readdir,
    .finddir = procfs_finddir,
    .mkdir   = NULL,
    .unlink  = NULL,
    .mount   = procfs_mount,
    .unmount = NULL,
};