/**
 * @file ext2.c
 * @brief Ext2-Dateisystem (read-only) für PinguinOS – Feature #50.
 *
 * Implementiert einen schreibgeschützten VFS-Backend für ext2-Volumes.
 * Unterstützt: open, read, readdir, finddir.
 * Nicht unterstützt: write, mkdir, unlink, Journaling (ext3/ext4).
 *
 * Ext2-Grundlagen:
 *   – Blockgröße: 1024 << s_log_block_size (typisch 1024 oder 4096 Byte)
 *   – Superblock: Byte-Offset 1024 vom Partitionsanfang
 *   – Block-Group-Descriptor-Tabelle: direkt nach dem Superblock-Block
 *   – Inodes: in Inode-Tabelle der jeweiligen Block-Gruppe
 *   – Verzeichniseinträge: variable Länge, 4-Byte-ausgerichtet
 *
 * Verwendung:
 *   vfs_register_fs(&ext2_ops);
 *   vfs_mount("/mnt/ext2", "ide0p1", "ext2", 0);
 */

#include "../include/vfs.h"
#include "../include/ide.h"    /* ide_read() */
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ══════════════════════════════════════════════════════════════════
 *  Ext2-Datenstrukturen (Little-Endian auf x86 nativ)
 * ══════════════════════════════════════════════════════════════════ */

/* ── Superblock (Offset 1024 Byte vom Partitionsanfang) ────────── */
typedef struct PACKED {
    uint32_t s_inodes_count;        /* Gesamtanzahl Inodes               */
    uint32_t s_blocks_count;        /* Gesamtanzahl Blöcke               */
    uint32_t s_r_blocks_count;      /* Für Root reservierte Blöcke       */
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;    /* 0 für 4KB-Blöcke, 1 für 1KB      */
    uint32_t s_log_block_size;      /* Blockgröße = 1024 << dieses Wert  */
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;               /* 0xEF53 = Ext2-Magic               */
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;           /* 0 = original, 1 = dynamic         */
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    /* Revision 1 Erweiterungen */
    uint32_t s_first_ino;           /* Erster verwendbarer Inode (11)    */
    uint16_t s_inode_size;          /* Inode-Größe (128 für Rev0)        */
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t  s_uuid[16];
    char     s_volume_name[16];
    char     s_last_mounted[64];
    uint32_t s_algo_bitmap;
    /* Padding auf 1024 Byte */
    uint8_t  s_padding[820];
} ext2_superblock_t;

#define EXT2_MAGIC  0xEF53

/* ── Block-Group-Deskriptor ────────────────────────────────────── */
typedef struct PACKED {
    uint32_t bg_block_bitmap;       /* Block-Bitmap-Block               */
    uint32_t bg_inode_bitmap;       /* Inode-Bitmap-Block               */
    uint32_t bg_inode_table;        /* Inode-Tabellen-Startblock        */
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint8_t  bg_reserved[12];
} ext2_bgd_t;

/* ── Inode ─────────────────────────────────────────────────────── */
typedef struct PACKED {
    uint16_t i_mode;                /* Dateityp und Berechtigungen       */
    uint16_t i_uid;
    uint32_t i_size;                /* Dateigröße in Bytes               */
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;              /* Anzahl 512-Byte-Blöcke            */
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];           /* 12 direkt + 1 ind. + 1 dind. + 1 tind. */
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t  i_osd2[12];
} ext2_inode_t;

/* ── Inode-Mode-Bits ───────────────────────────────────────────── */
#define EXT2_S_IFREG  0x8000   /* Reguläre Datei  */
#define EXT2_S_IFDIR  0x4000   /* Verzeichnis     */
#define EXT2_S_IFLNK  0xA000   /* Symbolischer Link */

/* ── Verzeichnis-Eintrag ───────────────────────────────────────── */
typedef struct PACKED {
    uint32_t inode;             /* Inode-Nummer (0 = ungültig)          */
    uint16_t rec_len;           /* Länge dieses Eintrags (4-Byte-align)  */
    uint8_t  name_len;          /* Länge des Namens in Bytes            */
    uint8_t  file_type;         /* Dateityp (1=Datei, 2=Verz., 7=Link)  */
    char     name[256];         /* Name (NICHT NUL-terminiert!)          */
} ext2_dirent_t;

/* ── Modul-Zustand ─────────────────────────────────────────────── */
#define EXT2_MAX_MOUNTS  4

typedef struct {
    bool              used;
    ext2_superblock_t sb;
    uint32_t          block_size;   /* Bytes pro Block                  */
    uint32_t          inodes_per_group;
    uint32_t          inode_size;
    uint32_t          first_data_block;
    uint32_t          num_groups;
    ext2_bgd_t       *bgdt;         /* Block-Group-Descriptor-Tabelle   */
    char              device[32];   /* Gerätename z. B. "ide0p1"        */
} ext2_mount_t;

static ext2_mount_t mounts[EXT2_MAX_MOUNTS];

/* ══════════════════════════════════════════════════════════════════
 *  Block-I/O-Hilfsfunktionen
 * ══════════════════════════════════════════════════════════════════ */

/**
 * @brief Einen ext2-Block in einen Puffer einlesen.
 *
 * Liest `block_size` Bytes ab dem physischen Offset
 * `block_nr * block_size` vom IDE-Gerät.
 */
static int ext2_read_block(ext2_mount_t *m, uint32_t block_nr, void *buf)
{
    /* Offset in Bytes vom Partitionsanfang */
    uint32_t byte_off  = block_nr * m->block_size;
    /* IDE arbeitet in 512-Byte-Sektoren */
    uint32_t lba_start = byte_off / 512;
    uint32_t sectors   = m->block_size / 512;

    /* Puffer für den IDE-Transfer (512-Byte-ausgerichtet) */
    for (uint32_t i = 0; i < sectors; i++) {
        uint8_t *dst = (uint8_t *)buf + i * 512;
        if (ide_read(0, lba_start + i, 1, dst) != 0)
            return -1;
    }
    return 0;
}

/* ── Inode aus der Inode-Tabelle lesen ─────────────────────────── */
static int ext2_read_inode(ext2_mount_t *m, uint32_t ino, ext2_inode_t *out)
{
    if (ino == 0) return -1;
    ino--;   /* Inodes sind 1-basiert */

    uint32_t group   = ino / m->inodes_per_group;
    uint32_t index   = ino % m->inodes_per_group;

    if (group >= m->num_groups) return -1;

    /* Block berechnen, in dem dieser Inode liegt */
    uint32_t inode_table_block = m->bgdt[group].bg_inode_table;
    uint32_t inodes_per_block  = m->block_size / m->inode_size;
    uint32_t block_off         = index / inodes_per_block;
    uint32_t block_idx         = index % inodes_per_block;

    uint8_t *block_buf = (uint8_t *)kmalloc(m->block_size);
    if (!block_buf) return -1;

    if (ext2_read_block(m, inode_table_block + block_off, block_buf) != 0) {
        kfree(block_buf);
        return -1;
    }

    memcpy(out, block_buf + block_idx * m->inode_size, sizeof(ext2_inode_t));
    kfree(block_buf);
    return 0;
}

/* ── Datenblock eines Inodes lesen (nur direkte Blöcke) ────────── */
static int ext2_read_inode_block(ext2_mount_t *m, ext2_inode_t *ino,
                                 uint32_t block_idx, void *buf)
{
    if (block_idx < 12) {
        /* Direkte Blöcke: i_block[0..11] */
        return ext2_read_block(m, ino->i_block[block_idx], buf);
    }

    /* Einfach indirekte Blöcke: i_block[12] */
    uint32_t ptrs_per_block = m->block_size / 4;
    uint32_t indirect_start = 12;
    uint32_t indirect_end   = indirect_start + ptrs_per_block;

    if (block_idx < indirect_end) {
        uint32_t *ind = (uint32_t *)kmalloc(m->block_size);
        if (!ind) return -1;
        if (ext2_read_block(m, ino->i_block[12], ind) != 0) {
            kfree(ind); return -1;
        }
        uint32_t real_block = ind[block_idx - indirect_start];
        kfree(ind);
        return ext2_read_block(m, real_block, buf);
    }

    /* Doppelt indirekte Blöcke: i_block[13] */
    uint32_t dind_start = indirect_end;
    uint32_t dind_end   = dind_start + ptrs_per_block * ptrs_per_block;

    if (block_idx < dind_end) {
        uint32_t *l1 = (uint32_t *)kmalloc(m->block_size);
        uint32_t *l2 = (uint32_t *)kmalloc(m->block_size);
        if (!l1 || !l2) { kfree(l1); kfree(l2); return -1; }

        if (ext2_read_block(m, ino->i_block[13], l1) != 0) {
            kfree(l1); kfree(l2); return -1;
        }
        uint32_t rel  = block_idx - dind_start;
        uint32_t idx1 = rel / ptrs_per_block;
        uint32_t idx2 = rel % ptrs_per_block;

        if (ext2_read_block(m, l1[idx1], l2) != 0) {
            kfree(l1); kfree(l2); return -1;
        }
        uint32_t real_block = l2[idx2];
        kfree(l1); kfree(l2);
        return ext2_read_block(m, real_block, buf);
    }

    /* Dreifach indirekte Blöcke: nicht implementiert (für >4 GB-Dateien) */
    KWARN("Ext2: dreifach indirekte Blöcke nicht unterstützt\n");
    return -1;
}

/* ══════════════════════════════════════════════════════════════════
 *  VFS-Operationen
 * ══════════════════════════════════════════════════════════════════ */

static int32_t ext2_vfs_read(vfs_node_t *node, uint32_t offset,
                              uint32_t size, uint8_t *buf)
{
    ext2_mount_t *m = (ext2_mount_t *)node->fs_data;
    if (!m) return -1;

    ext2_inode_t ino;
    if (ext2_read_inode(m, node->inode_num, &ino) != 0) return -1;

    if (offset >= ino.i_size) return 0;
    if (offset + size > ino.i_size)
        size = ino.i_size - offset;

    uint8_t *block_buf = (uint8_t *)kmalloc(m->block_size);
    if (!block_buf) return -1;

    uint32_t bytes_read = 0;
    while (bytes_read < size) {
        uint32_t file_off   = offset + bytes_read;
        uint32_t block_idx  = file_off / m->block_size;
        uint32_t block_off  = file_off % m->block_size;
        uint32_t can_read   = m->block_size - block_off;
        if (can_read > size - bytes_read)
            can_read = size - bytes_read;

        if (ext2_read_inode_block(m, &ino, block_idx, block_buf) != 0)
            break;

        memcpy(buf + bytes_read, block_buf + block_off, can_read);
        bytes_read += can_read;
    }

    kfree(block_buf);
    return (int32_t)bytes_read;
}

static bool ext2_vfs_readdir(vfs_node_t *node, uint32_t idx,
                              vfs_dirent_t *out)
{
    ext2_mount_t *m = (ext2_mount_t *)node->fs_data;
    if (!m) return false;

    ext2_inode_t ino;
    if (ext2_read_inode(m, node->inode_num, &ino) != 0) return false;
    if (!(ino.i_mode & EXT2_S_IFDIR)) return false;

    uint8_t *block_buf = (uint8_t *)kmalloc(m->block_size);
    if (!block_buf) return false;

    uint32_t entry_count = 0;
    bool     found       = false;

    /* Alle Datenblöcke des Verzeichnisses durchlaufen */
    for (uint32_t bi = 0; bi < 12 && !found; bi++) {
        if (ino.i_block[bi] == 0) break;
        if (ext2_read_inode_block(m, &ino, bi, block_buf) != 0) break;

        uint32_t pos = 0;
        while (pos < m->block_size) {
            ext2_dirent_t *de = (ext2_dirent_t *)(void *)(block_buf + pos);
            if (de->rec_len == 0) break;

            if (de->inode != 0) {
                if (entry_count == idx) {
                    uint32_t nlen = de->name_len < 255 ? de->name_len : 255;
                    memcpy(out->name, de->name, nlen);
                    out->name[nlen] = '\0';
                    out->inode_num  = de->inode;
                    out->type = (de->file_type == 2) ? VFS_DIR : VFS_FILE;
                    found = true;
                    break;
                }
                entry_count++;
            }
            pos += de->rec_len;
        }
    }

    kfree(block_buf);
    return found;
}

static vfs_node_t *ext2_vfs_finddir(vfs_node_t *parent, const char *name)
{
    ext2_mount_t *m = (ext2_mount_t *)parent->fs_data;
    if (!m) return NULL;

    ext2_inode_t ino;
    if (ext2_read_inode(m, parent->inode_num, &ino) != 0) return NULL;

    uint8_t *block_buf = (uint8_t *)kmalloc(m->block_size);
    if (!block_buf) return NULL;

    vfs_node_t *result = NULL;

    for (uint32_t bi = 0; bi < 12 && !result; bi++) {
        if (ino.i_block[bi] == 0) break;
        if (ext2_read_inode_block(m, &ino, bi, block_buf) != 0) break;

        uint32_t pos = 0;
        while (pos < m->block_size) {
            ext2_dirent_t *de = (ext2_dirent_t *)(void *)(block_buf + pos);
            if (de->rec_len == 0) break;

            if (de->inode != 0) {
                uint32_t nlen = de->name_len < 255 ? de->name_len : 255;
                if (strncmp(de->name, name, nlen) == 0 &&
                    name[nlen] == '\0') {
                    /* Inode der gefundenen Datei laden */
                    ext2_inode_t child_ino;
                    if (ext2_read_inode(m, de->inode, &child_ino) == 0) {
                        result = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
                        if (result) {
                            strncpy(result->name, name, 255);
                            result->inode_num = de->inode;
                            result->size      = child_ino.i_size;
                            result->flags     = (de->file_type == 2)
                                                ? VFS_DIR : VFS_FILE;
                            result->fs_data   = m;
                            result->ops       = &ext2_ops;
                        }
                    }
                    break;
                }
            }
            pos += de->rec_len;
        }
    }

    kfree(block_buf);
    return result;
}

/* ── Mount-Funktion ────────────────────────────────────────────── */
static vfs_node_t *ext2_vfs_mount(const char *device, uint32_t flags)
{
    (void)flags;

    /* Freien Mount-Slot suchen */
    ext2_mount_t *m = NULL;
    for (int i = 0; i < EXT2_MAX_MOUNTS; i++) {
        if (!mounts[i].used) { m = &mounts[i]; break; }
    }
    if (!m) { KERR("Ext2: Keine Mount-Slots mehr frei\n"); return NULL; }

    /* Superblock einlesen (Byte-Offset 1024 = Sektor 2 bei 512-B-Sektoren) */
    uint8_t sb_buf[1024];
    /* Sektor 2 und 3 lesen (zusammen 1024 Byte) */
    if (ide_read(0, 2, 1, sb_buf)      != 0 ||
        ide_read(0, 3, 1, sb_buf + 512) != 0) {
        KERR("Ext2: Superblock-Lesefehler auf '%s'\n", device);
        return NULL;
    }

    memcpy(&m->sb, sb_buf, sizeof(ext2_superblock_t));

    /* Magic prüfen */
    if (m->sb.s_magic != EXT2_MAGIC) {
        KERR("Ext2: Ungültiges Magic 0x%04x auf '%s'\n",
             m->sb.s_magic, device);
        return NULL;
    }

    m->block_size       = 1024u << m->sb.s_log_block_size;
    m->inodes_per_group = m->sb.s_inodes_per_group;
    m->inode_size       = (m->sb.s_rev_level >= 1)
                          ? m->sb.s_inode_size : 128;
    m->first_data_block = m->sb.s_first_data_block;
    m->num_groups       = (m->sb.s_blocks_count +
                           m->sb.s_blocks_per_group - 1)
                          / m->sb.s_blocks_per_group;
    strncpy(m->device, device, 31);

    /* Block-Group-Descriptor-Tabelle einlesen */
    uint32_t bgdt_block = m->first_data_block + 1;
    uint32_t bgdt_size  = m->num_groups * sizeof(ext2_bgd_t);
    /* Aufrunden auf ganze Blöcke */
    uint32_t bgdt_blocks = (bgdt_size + m->block_size - 1) / m->block_size;

    m->bgdt = (ext2_bgd_t *)kmalloc(bgdt_blocks * m->block_size);
    if (!m->bgdt) { KERR("Ext2: Kein Speicher für BGDT\n"); return NULL; }

    for (uint32_t i = 0; i < bgdt_blocks; i++) {
        if (ext2_read_block(m, bgdt_block + i,
                            (uint8_t *)m->bgdt + i * m->block_size) != 0) {
            kfree(m->bgdt);
            KERR("Ext2: BGDT-Lesefehler\n");
            return NULL;
        }
    }

    m->used = true;

    KINFO("Ext2: '%s' eingehängt  Blöcke=%u  Inodes=%u  BS=%u  Gruppen=%u\n",
          device, m->sb.s_blocks_count, m->sb.s_inodes_count,
          m->block_size, m->num_groups);

    /* Wurzel-Inode (Inode 2) als VFS-Knoten zurückgeben */
    ext2_inode_t root_ino;
    if (ext2_read_inode(m, 2, &root_ino) != 0) {
        kfree(m->bgdt); m->used = false;
        KERR("Ext2: Wurzel-Inode nicht lesbar\n");
        return NULL;
    }

    vfs_node_t *root = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!root) { kfree(m->bgdt); m->used = false; return NULL; }

    memcpy(root->name, "/", 2);
    root->inode_num = 2;
    root->flags     = VFS_DIR;
    root->size      = root_ino.i_size;
    root->fs_data   = m;
    root->ops       = &ext2_ops;
    return root;
}

static void ext2_vfs_unmount(vfs_node_t *root)
{
    if (!root) return;
    ext2_mount_t *m = (ext2_mount_t *)root->fs_data;
    if (m) {
        kfree(m->bgdt);
        m->used = false;
    }
}

/* ── VFS-Operationstabelle ─────────────────────────────────────── */
fs_ops_t ext2_ops = {
    .name    = "ext2",
    .read    = ext2_vfs_read,
    .write   = NULL,           /* Read-only */
    .open    = NULL,
    .close   = NULL,
    .readdir = ext2_vfs_readdir,
    .finddir = ext2_vfs_finddir,
    .mkdir   = NULL,
    .unlink  = NULL,
    .mount   = ext2_vfs_mount,
    .unmount = ext2_vfs_unmount,
};