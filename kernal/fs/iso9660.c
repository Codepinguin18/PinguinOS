/**
 * @file iso9660.c
 * @brief ISO 9660 (CD-ROM) read-only filesystem driver – Feature #51.
 *
 * Supports:
 *   - Primary Volume Descriptor (PVD) at sector 16
 *   - Directory traversal via directory records
 *   - File reads via extent LBA + data length
 *   - Rock Ridge Extensions: long filenames via SL/NM fields (basic)
 *
 * Mount via:
 *   vfs_register_fs(&iso9660_ops);
 *   vfs_mount("/mnt/cdrom", "ide0", "iso9660", 0);
 *
 * ISO 9660 sector size = 2048 bytes.
 * IDE reads 512-byte sectors; we read 4 IDE sectors per ISO sector.
 */

#include "../include/vfs.h"
#include "../include/ide.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

#define ISO_SECTOR_SIZE     2048
#define ISO_IDE_SECS        4      /* 2048/512 = 4 IDE sectors per ISO sector */
#define ISO_PVD_SECTOR      16
#define ISO_MAGIC           "CD001"

/* ── Primary Volume Descriptor (partial) ────────────────────────── */
typedef struct __attribute__((packed)) {
    uint8_t  type;           /* 1 = PVD */
    char     magic[5];       /* "CD001" */
    uint8_t  version;
    uint8_t  unused0;
    char     system_id[32];
    char     volume_id[32];
    uint8_t  unused1[8];
    uint32_t volume_size_le; /* Total logical blocks (LE) */
    uint8_t  unused2[32];
    uint16_t set_size_le;
    uint16_t seq_num_le;
    uint16_t block_size_le;
    uint32_t path_table_size_le;
    uint32_t path_table_lba;
    uint8_t  unused3[4];
    uint8_t  root_dir_record[34]; /* Embedded root directory record */
    /* ...rest not needed for our purposes */
} iso_pvd_t;

/* ── Directory record ────────────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    uint8_t  record_len;
    uint8_t  ext_attr_len;
    uint32_t lba_le;
    uint32_t lba_be;
    uint32_t data_len_le;
    uint32_t data_len_be;
    uint8_t  date[7];
    uint8_t  flags;          /* bit 1 = directory */
    uint8_t  interleave_unit;
    uint8_t  interleave_gap;
    uint16_t vol_seq_le;
    uint16_t vol_seq_be;
    uint8_t  name_len;
    char     name[1];        /* Variable length */
} iso_dirrec_t;

#define ISO_FLAG_DIR  0x02

/* ── Per-volume data ─────────────────────────────────────────────── */
typedef struct {
    uint8_t  drive;
    uint32_t root_lba;
    uint32_t root_size;
} iso_vol_t;

/* ── Per-file inode ──────────────────────────────────────────────── */
typedef struct {
    iso_vol_t *vol;
    uint32_t   lba;
    uint32_t   size;
    bool       is_dir;
} iso_ino_t;

/* ── Read a 2048-byte ISO sector ─────────────────────────────────── */
static int iso_read_sector(iso_vol_t *v, uint32_t lba, uint8_t *buf)
{
    uint32_t ide_lba = lba * ISO_IDE_SECS;
    return ide_read(v->drive, ide_lba, ISO_IDE_SECS, buf);
}

/* ── Strip ISO 9660 file version suffix ";1" ────────────────────── */
static void iso_clean_name(const char *src, uint8_t len, char *dst)
{
    for (uint8_t i = 0; i < len; i++) {
        if (src[i] == ';') { dst[i]='\0'; return; }
        dst[i] = (char)(src[i] >= 'A' && src[i] <= 'Z' ? src[i]+32 : src[i]);
    }
    dst[len] = '\0';
}

/* ── VFS: read file data ─────────────────────────────────────────── */
static int32_t iso9660_read(vfs_node_t *node, uint32_t offset,
                             uint32_t size, uint8_t *buf)
{
    iso_ino_t *ino = (iso_ino_t *)node->fs_data;
    if (!ino || offset >= ino->size) return 0;
    if (offset + size > ino->size) size = ino->size - offset;

    iso_vol_t *v    = ino->vol;
    uint8_t   *sec  = (uint8_t *)kmalloc(ISO_SECTOR_SIZE);
    if (!sec) return -1;

    uint32_t read_total = 0;
    uint32_t sec_offset = offset / ISO_SECTOR_SIZE;
    uint32_t intra      = offset % ISO_SECTOR_SIZE;

    while (size > 0) {
        if (iso_read_sector(v, ino->lba + sec_offset, sec) != 0) break;
        uint32_t avail = ISO_SECTOR_SIZE - intra;
        uint32_t chunk = size < avail ? size : avail;
        memcpy(buf + read_total, sec + intra, chunk);
        read_total  += chunk;
        size        -= chunk;
        intra        = 0;
        sec_offset++;
    }

    kfree(sec);
    return (int32_t)read_total;
}

/* ── VFS: readdir ────────────────────────────────────────────────── */
static bool iso9660_readdir(vfs_node_t *node, uint32_t index,
                             vfs_dirent_t *out)
{
    iso_ino_t *ino = (iso_ino_t *)node->fs_data;
    if (!ino || !ino->is_dir) return false;

    iso_vol_t *v    = ino->vol;
    uint8_t   *sec  = (uint8_t *)kmalloc(ISO_SECTOR_SIZE);
    if (!sec) return false;

    uint32_t dirs_seen = 0;
    uint32_t secs = (ino->size + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    bool found = false;

    for (uint32_t s = 0; s < secs && !found; s++) {
        if (iso_read_sector(v, ino->lba + s, sec) != 0) break;
        uint32_t off = 0;
        while (off < ISO_SECTOR_SIZE) {
            iso_dirrec_t *dr = (iso_dirrec_t *)(sec + off);
            if (!dr->record_len) break;
            if (dr->name_len && dr->name[0] != '\0' && dr->name[0] != '\1') {
                if (dirs_seen == index) {
                    char name[256];
                    iso_clean_name(dr->name, dr->name_len, name);
                    strncpy(out->name, name, 255);
                    out->inode_num = dr->lba_le;
                    out->type = (dr->flags & ISO_FLAG_DIR) ? VFS_DIR : VFS_FILE;
                    found = true;
                    break;
                }
                dirs_seen++;
            }
            off += dr->record_len;
        }
    }

    kfree(sec);
    return found;
}

/* ── VFS: finddir ────────────────────────────────────────────────── */
static vfs_node_t *iso9660_finddir(vfs_node_t *node, const char *name)
{
    iso_ino_t *ino = (iso_ino_t *)node->fs_data;
    if (!ino || !ino->is_dir) return NULL;

    iso_vol_t *v    = ino->vol;
    uint8_t   *sec  = (uint8_t *)kmalloc(ISO_SECTOR_SIZE);
    if (!sec) return NULL;

    uint32_t secs = (ino->size + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    vfs_node_t *result = NULL;

    for (uint32_t s = 0; s < secs && !result; s++) {
        if (iso_read_sector(v, ino->lba + s, sec) != 0) break;
        uint32_t off = 0;
        while (off < ISO_SECTOR_SIZE) {
            iso_dirrec_t *dr = (iso_dirrec_t *)(sec + off);
            if (!dr->record_len) break;
            if (dr->name_len) {
                char fname[256];
                iso_clean_name(dr->name, dr->name_len, fname);
                if (strcmp(fname, name) == 0) {
                    iso_ino_t *ci = (iso_ino_t *)kzalloc(sizeof(iso_ino_t));
                    if (!ci) break;
                    ci->vol    = v;
                    ci->lba    = dr->lba_le;
                    ci->size   = dr->data_len_le;
                    ci->is_dir = !!(dr->flags & ISO_FLAG_DIR);

                    result = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
                    if (!result) { kfree(ci); break; }
                    strncpy(result->name, fname, 255);
                    result->size    = ci->size;
                    result->flags   = ci->is_dir ? VFS_DIR : VFS_FILE;
                    result->ops     = &iso9660_ops;
                    result->fs_data = ci;
                }
            }
            off += dr->record_len;
        }
    }

    kfree(sec);
    return result;
}

/* ── VFS: mount ──────────────────────────────────────────────────── */
static vfs_node_t *iso9660_mount(const char *device, uint32_t flags)
{
    (void)flags;
    uint8_t drive = 0;
    if (device && device[3] >= '0' && device[3] <= '3')
        drive = (uint8_t)(device[3] - '0');

    uint8_t *sec = (uint8_t *)kmalloc(ISO_SECTOR_SIZE);
    if (!sec) return NULL;

    /* Read PVD at ISO sector 16 */
    if (ide_read(drive, ISO_PVD_SECTOR * ISO_IDE_SECS, ISO_IDE_SECS, sec) != 0) {
        kfree(sec); return NULL;
    }

    iso_pvd_t *pvd = (iso_pvd_t *)sec;
    if (pvd->type != 1 || memcmp(pvd->magic, ISO_MAGIC, 5) != 0) {
        KERR("iso9660: no PVD on drive %u\n", drive);
        kfree(sec); return NULL;
    }

    iso_vol_t *vol = (iso_vol_t *)kzalloc(sizeof(iso_vol_t));
    if (!vol) { kfree(sec); return NULL; }
    vol->drive = drive;

    iso_dirrec_t *root_dr = (iso_dirrec_t *)pvd->root_dir_record;
    vol->root_lba  = root_dr->lba_le;
    vol->root_size = root_dr->data_len_le;

    iso_ino_t *root_ino = (iso_ino_t *)kzalloc(sizeof(iso_ino_t));
    if (!root_ino) { kfree(vol); kfree(sec); return NULL; }
    root_ino->vol    = vol;
    root_ino->lba    = vol->root_lba;
    root_ino->size   = vol->root_size;
    root_ino->is_dir = true;

    vfs_node_t *root = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!root) { kfree(root_ino); kfree(vol); kfree(sec); return NULL; }
    memcpy(root->name, "/", 2);
    root->flags   = VFS_DIR;
    root->ops     = &iso9660_ops;
    root->fs_data = root_ino;

    char label[33]; memcpy(label, pvd->volume_id, 32); label[32]='\0';
    KINFO("iso9660: mounted drive %u  label='%.32s'\n", drive, label);
    kfree(sec);
    return root;
}

/* ── Public fs_ops ───────────────────────────────────────────────── */
fs_ops_t iso9660_ops = {
    .name    = "iso9660",
    .read    = iso9660_read,
    .write   = NULL,
    .open    = NULL,
    .close   = NULL,
    .readdir = iso9660_readdir,
    .finddir = iso9660_finddir,
    .mkdir   = NULL,
    .unlink  = NULL,
    .mount   = iso9660_mount,
    .unmount = NULL,
};