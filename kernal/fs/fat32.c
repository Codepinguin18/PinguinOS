/**
 * @file fat32.c
 * @brief FAT32 read-only filesystem driver for PinguinOS.
 *
 * Feature #49 – FAT32
 *
 * Implements a read-only VFS backend for FAT32 volumes.
 * Supports: open, read, readdir, finddir.
 * Does NOT support: write, mkdir, unlink, long-file-name (LFN) entries
 * beyond reading their short-name equivalent.
 *
 * Usage:
 *   vfs_register_fs(&fat32_ops);
 *   vfs_mount("/mnt/disk", "ide0p0", "fat32", 0);
 */

#include "../include/vfs.h"
#include "../include/ide.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── BPB (BIOS Parameter Block) ─────────────────────────────────── */
typedef struct __attribute__((packed)) {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_entry_count;    /* 0 for FAT32 */
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t fat_size_16;         /* 0 for FAT32 */
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    /* FAT32 extended */
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t version;
    uint32_t root_cluster;        /* Usually cluster 2 */
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     fs_type[8];          /* "FAT32   " */
} fat32_bpb_t;

/* ── Directory entry ─────────────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    char     name[8];
    char     ext[3];
    uint8_t  attrib;
    uint8_t  reserved;
    uint8_t  create_time_tenths;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t cluster_low;
    uint32_t file_size;
} fat32_dirent_t;

#define FAT32_ATTR_READ_ONLY  0x01
#define FAT32_ATTR_HIDDEN     0x02
#define FAT32_ATTR_SYSTEM     0x04
#define FAT32_ATTR_VOLUME_ID  0x08
#define FAT32_ATTR_DIRECTORY  0x10
#define FAT32_ATTR_ARCHIVE    0x20
#define FAT32_ATTR_LFN        0x0F   /* Long filename entry */

#define FAT32_EOC  0x0FFFFFF8u   /* End-of-cluster chain */
#define FAT32_FREE 0x00000000u

/* ── Per-volume mount data ───────────────────────────────────────── */
typedef struct {
    uint8_t  drive;
    uint32_t fat_start;       /* LBA of first FAT sector */
    uint32_t data_start;      /* LBA of first data cluster */
    uint32_t sectors_per_cluster;
    uint32_t bytes_per_cluster;
    uint32_t root_cluster;
    uint32_t bytes_per_sector;
} fat32_vol_t;

/* ── Per-file/dir inode data ─────────────────────────────────────── */
typedef struct {
    fat32_vol_t *vol;
    uint32_t     first_cluster;
    uint32_t     file_size;
} fat32_inode_t;

/* ── Helpers ─────────────────────────────────────────────────────── */
static uint32_t cluster_to_lba(const fat32_vol_t *v, uint32_t cluster)
{
    return v->data_start + (cluster - 2) * v->sectors_per_cluster;
}

static uint32_t fat32_next_cluster(const fat32_vol_t *v, uint32_t cluster)
{
    /* Each FAT32 entry is 4 bytes; read the sector containing it */
    uint32_t fat_offset  = cluster * 4;
    uint32_t fat_sector  = v->fat_start + fat_offset / v->bytes_per_sector;
    uint32_t entry_offset= fat_offset % v->bytes_per_sector;

    uint8_t sector[512];
    if (ide_read(v->drive, fat_sector, 1, sector) != 0) return FAT32_EOC;

    uint32_t val;
    memcpy(&val, sector + entry_offset, 4);
    return val & 0x0FFFFFFFu;
}

/* Read @p n_sectors sectors starting at LBA @p lba into @p buf. */
static int fat32_read_sectors(const fat32_vol_t *v, uint32_t lba,
                               uint32_t n_sectors, uint8_t *buf)
{
    while (n_sectors) {
        uint8_t batch = (n_sectors > 255) ? 255 : (uint8_t)n_sectors;
        if (ide_read(v->drive, lba, batch, buf) != 0) return -1;
        lba       += batch;
        buf       += batch * v->bytes_per_sector;
        n_sectors -= batch;
    }
    return 0;
}

/* Build an 8.3 short name string from a directory entry. */
static void fat32_short_name(const fat32_dirent_t *de, char *out)
{
    int i = 0;
    for (int j = 0; j < 8 && de->name[j] != ' '; j++) out[i++] = de->name[j];
    if (de->ext[0] != ' ') {
        out[i++] = '.';
        for (int j = 0; j < 3 && de->ext[j] != ' '; j++) out[i++] = de->ext[j];
    }
    out[i] = '\0';
    /* Convert to lowercase for POSIX feel */
    for (int j = 0; j < i; j++)
        if (out[j] >= 'A' && out[j] <= 'Z') out[j] += 32;
}

/* ── VFS operations ──────────────────────────────────────────────── */

static int32_t fat32_read(vfs_node_t *node, uint32_t offset,
                           uint32_t size, uint8_t *buf)
{
    fat32_inode_t *ino = (fat32_inode_t *)node->fs_data;
    if (!ino || offset >= ino->file_size) return 0;

    fat32_vol_t *v = ino->vol;
    if (offset + size > ino->file_size) size = ino->file_size - offset;

    /* Walk cluster chain to the right cluster */
    uint32_t cluster_idx = offset / v->bytes_per_cluster;
    uint32_t cluster     = ino->first_cluster;
    for (uint32_t i = 0; i < cluster_idx; i++) {
        cluster = fat32_next_cluster(v, cluster);
        if (cluster >= FAT32_EOC) return 0;
    }

    uint32_t intra_cluster = offset % v->bytes_per_cluster;
    uint32_t read_total    = 0;
    uint8_t *cluster_buf   = (uint8_t *)kmalloc(v->bytes_per_cluster);
    if (!cluster_buf) return -1;

    while (size > 0 && cluster < FAT32_EOC) {
        if (fat32_read_sectors(v, cluster_to_lba(v, cluster),
                               v->sectors_per_cluster, cluster_buf) != 0) break;

        uint32_t avail = v->bytes_per_cluster - intra_cluster;
        uint32_t chunk = (size < avail) ? size : avail;
        memcpy(buf + read_total, cluster_buf + intra_cluster, chunk);

        read_total     += chunk;
        size           -= chunk;
        intra_cluster   = 0;
        cluster         = fat32_next_cluster(v, cluster);
    }

    kfree(cluster_buf);
    return (int32_t)read_total;
}

static bool fat32_readdir(vfs_node_t *node, uint32_t index,
                           vfs_dirent_t *out)
{
    fat32_inode_t *ino = (fat32_inode_t *)node->fs_data;
    if (!ino) return false;
    fat32_vol_t *v = ino->vol;

    uint8_t *cluster_buf = (uint8_t *)kmalloc(v->bytes_per_cluster);
    if (!cluster_buf) return false;

    uint32_t cluster   = ino->first_cluster;
    uint32_t entry_idx = 0;
    bool     found     = false;

    while (cluster < FAT32_EOC && !found) {
        if (fat32_read_sectors(v, cluster_to_lba(v, cluster),
                               v->sectors_per_cluster, cluster_buf) != 0) break;

        uint32_t entries = v->bytes_per_cluster / sizeof(fat32_dirent_t);
        for (uint32_t i = 0; i < entries && !found; i++) {
            fat32_dirent_t *de = (fat32_dirent_t *)(cluster_buf + i * 32);
            if ((uint8_t)de->name[0] == 0x00) goto done;
            if ((uint8_t)de->name[0] == 0xE5) continue;     /* Deleted */
            if (de->attrib == FAT32_ATTR_LFN)   continue;   /* LFN entry */
            if (de->attrib & FAT32_ATTR_VOLUME_ID) continue;

            if (entry_idx == index) {
                char sname[13];
                fat32_short_name(de, sname);
                strncpy(out->name, sname, 255);
                out->inode_num = ((uint32_t)de->cluster_high << 16) | de->cluster_low;
                out->type      = (de->attrib & FAT32_ATTR_DIRECTORY) ? VFS_DIR : VFS_FILE;
                found = true;
            }
            entry_idx++;
        }
        cluster = fat32_next_cluster(v, cluster);
    }
done:
    kfree(cluster_buf);
    return found;
}

static vfs_node_t *fat32_finddir(vfs_node_t *node, const char *name)
{
    fat32_inode_t *ino = (fat32_inode_t *)node->fs_data;
    if (!ino) return NULL;
    fat32_vol_t *v = ino->vol;

    uint8_t *cluster_buf = (uint8_t *)kmalloc(v->bytes_per_cluster);
    if (!cluster_buf) return NULL;

    uint32_t    cluster = ino->first_cluster;
    vfs_node_t *result  = NULL;

    while (cluster < FAT32_EOC && !result) {
        if (fat32_read_sectors(v, cluster_to_lba(v, cluster),
                               v->sectors_per_cluster, cluster_buf) != 0) break;

        uint32_t entries = v->bytes_per_cluster / sizeof(fat32_dirent_t);
        for (uint32_t i = 0; i < entries && !result; i++) {
            fat32_dirent_t *de = (fat32_dirent_t *)(cluster_buf + i * 32);
            if ((uint8_t)de->name[0] == 0x00) goto done2;
            if ((uint8_t)de->name[0] == 0xE5) continue;
            if (de->attrib == FAT32_ATTR_LFN)  continue;
            if (de->attrib & FAT32_ATTR_VOLUME_ID) continue;

            char sname[13];
            fat32_short_name(de, sname);
            if (strncmp(sname, name, 12) != 0) continue;

            fat32_inode_t *child_ino = (fat32_inode_t *)kmalloc(sizeof(fat32_inode_t));
            if (!child_ino) goto done2;
            child_ino->vol           = v;
            child_ino->first_cluster = ((uint32_t)de->cluster_high << 16) | de->cluster_low;
            child_ino->file_size     = de->file_size;

            result = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
            if (!result) { kfree(child_ino); goto done2; }
            strncpy(result->name, sname, 255);
            result->size    = de->file_size;
            result->flags   = (de->attrib & FAT32_ATTR_DIRECTORY) ? VFS_DIR : VFS_FILE;
            result->ops     = &fat32_ops;
            result->fs_data = child_ino;
        }
        cluster = fat32_next_cluster(v, cluster);
    }
done2:
    kfree(cluster_buf);
    return result;
}

static vfs_node_t *fat32_mount(const char *device, uint32_t flags)
{
    (void)flags;
    /* device string "ideXpY" → drive X, partition Y */
    uint8_t drive = 0;
    if (device && device[3] >= '0' && device[3] <= '3')
        drive = (uint8_t)(device[3] - '0');

    uint8_t sector[512];
    if (ide_read(drive, 0, 1, sector) != 0) return NULL;

    fat32_bpb_t *bpb = (fat32_bpb_t *)sector;

    /* Quick sanity checks */
    if (bpb->bytes_per_sector != 512 && bpb->bytes_per_sector != 1024 &&
        bpb->bytes_per_sector != 2048 && bpb->bytes_per_sector != 4096)
        return NULL;
    if (bpb->fat_size_32 == 0) return NULL;   /* Not FAT32 */

    fat32_vol_t *vol = (fat32_vol_t *)kzalloc(sizeof(fat32_vol_t));
    if (!vol) return NULL;

    vol->drive              = drive;
    vol->bytes_per_sector   = bpb->bytes_per_sector;
    vol->sectors_per_cluster= bpb->sectors_per_cluster;
    vol->bytes_per_cluster  = (uint32_t)bpb->bytes_per_sector * bpb->sectors_per_cluster;
    vol->fat_start          = bpb->reserved_sectors;
    vol->data_start         = bpb->reserved_sectors +
                              (uint32_t)bpb->fat_count * bpb->fat_size_32;
    vol->root_cluster       = bpb->root_cluster;

    fat32_inode_t *root_ino = (fat32_inode_t *)kzalloc(sizeof(fat32_inode_t));
    if (!root_ino) { kfree(vol); return NULL; }
    root_ino->vol           = vol;
    root_ino->first_cluster = vol->root_cluster;
    root_ino->file_size     = 0;

    vfs_node_t *root = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!root) { kfree(root_ino); kfree(vol); return NULL; }
    memcpy(root->name, "/", 2);
    root->flags   = VFS_DIR;
    root->ops     = &fat32_ops;
    root->fs_data = root_ino;

    char label[12];
    memcpy(label, bpb->volume_label, 11);
    label[11] = '\0';
    KINFO("FAT32: mounted drive %u  label='%s'  cluster_size=%u B\n",
          drive, label, vol->bytes_per_cluster);
    return root;
}

/* ── Public fs_ops ───────────────────────────────────────────────── */
fs_ops_t fat32_ops = {
    .name    = "fat32",
    .read    = fat32_read,
    .write   = NULL,       /* Read-only */
    .open    = NULL,
    .close   = NULL,
    .readdir = fat32_readdir,
    .finddir = fat32_finddir,
    .mkdir   = NULL,
    .unlink  = NULL,
    .mount   = fat32_mount,
    .unmount = NULL,
};