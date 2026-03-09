/**
 * @file vfs.h
 * @brief Virtual File System Layer (Feature #46) für PinguinOS.
 *
 * Der VFS-Layer abstrahiert konkrete Dateisystem-Implementierungen
 * hinter einem einheitlichen Interface. Jedes Dateisystem registriert
 * einen filesystem_ops_t-Block; der VFS-Layer leitet Operationen
 * (open, read, write, readdir, …) an das zuständige FS weiter.
 *
 * Hierarchie:
 *   vfs_node_t  – Inode-ähnlich; repräsentiert Datei/Verzeichnis
 *   vfs_mount_t – Mount-Punkt (Dateisystem + Wurzelknoten)
 *   fs_ops_t    – Vtable für ein konkretes Dateisystem
 */
#ifndef _VFS_H
#define _VFS_H

#include "types.h"

/* ── Knotentypen ─────────────────────────────────────────────────── */
#define VFS_FILE      0x01
#define VFS_DIR       0x02
#define VFS_SYMLINK   0x04
#define VFS_CHARDEV   0x08
#define VFS_BLOCKDEV  0x10
#define VFS_PIPE      0x20
#define VFS_MOUNTPT   0x40

/* ── Zugriffsflags für vfs_open ─────────────────────────────────── */
#define O_RDONLY   0x000
#define O_WRONLY   0x001
#define O_RDWR     0x002
#define O_CREAT    0x040
#define O_TRUNC    0x200
#define O_APPEND   0x400

/* ── Vorwärtsdeklarationen ───────────────────────────────────────── */
typedef struct vfs_node      vfs_node_t;
typedef struct vfs_dirent    vfs_dirent_t;
typedef struct fs_ops        fs_ops_t;
typedef struct vfs_mount     vfs_mount_t;

/* ── Verzeichnis-Eintrag (für readdir) ───────────────────────────── */
struct vfs_dirent {
    char     name[256];
    uint32_t inode_num;
    uint8_t  type;         /* VFS_* Knotentyp */
};

/* ── Dateisystem-Operationen (vtable) ───────────────────────────── */
struct fs_ops {
    const char *name;

    /* Datei-Operationen */
    int32_t  (*read) (vfs_node_t *node, uint32_t offset,
                      uint32_t size, uint8_t *buf);
    int32_t  (*write)(vfs_node_t *node, uint32_t offset,
                      uint32_t size, const uint8_t *buf);
    vfs_node_t *(*open) (vfs_node_t *node, const char *name, uint32_t flags);
    void     (*close)(vfs_node_t *node);

    /* Verzeichnis-Operationen */
    bool     (*readdir)(vfs_node_t *node, uint32_t index,
                        vfs_dirent_t *out);
    vfs_node_t *(*finddir)(vfs_node_t *node, const char *name);
    int      (*mkdir)(vfs_node_t *node, const char *name, uint32_t flags);
    int      (*unlink)(vfs_node_t *node, const char *name);

    /* Mount/Unmount */
    vfs_node_t *(*mount)(const char *device, uint32_t flags);
    void     (*unmount)(vfs_node_t *root);
};

/* ── VFS-Knoten ──────────────────────────────────────────────────── */
struct vfs_node {
    char       name[256];
    uint32_t   inode_num;
    uint32_t   flags;        /* VFS_* Knotentyp                    */
    uint32_t   size;         /* Dateigröße in Bytes                */
    uint32_t   uid, gid;
    uint32_t   permissions;  /* Unix rwxrwxrwx                     */
    uint32_t   atime, mtime, ctime;
    fs_ops_t  *ops;          /* Zeiger auf FS-vtable               */
    void      *fs_data;      /* FS-private Daten                   */
    vfs_node_t *mountpoint;  /* != NULL wenn hier gemountet        */
    uint32_t   ref_count;
};

/* ── Mount-Tabellen-Eintrag ──────────────────────────────────────── */
#define VFS_MAX_MOUNTS  16
#define VFS_PATH_MAX   256

struct vfs_mount {
    char        path[VFS_PATH_MAX];   /* Mount-Punkt-Pfad           */
    vfs_node_t *root;                 /* Wurzelknoten des FS        */
    fs_ops_t   *ops;
    bool        used;
};

/* ── Öffentliche API ─────────────────────────────────────────────── */

/** VFS-System initialisieren. */
void vfs_init(void);

/** DevFS initialisieren. */
void devfs_init(void);

/** Dateisystem registrieren. */
void vfs_register_fs(fs_ops_t *ops);

/** Dateisystem an einem Pfad einhängen. */
int vfs_mount(const char *path, const char *device, const char *fsname, uint32_t flags);

/** Dateisystem aushängen. */
int vfs_umount(const char *path);

/** Pfad auflösen → vfs_node_t */
vfs_node_t *vfs_lookup(const char *path);

/** Datei öffnen. */
vfs_node_t *vfs_open(const char *path, uint32_t flags);

/** Datei schließen. */
void vfs_close(vfs_node_t *node);

/** Aus einer Datei lesen. */
int32_t vfs_read(vfs_node_t *node, uint32_t offset,
                 uint32_t size, uint8_t *buf);

/** In eine Datei schreiben. */
int32_t vfs_write(vfs_node_t *node, uint32_t offset,
                  uint32_t size, const uint8_t *buf);

/** Verzeichnis-Eintrag lesen. */
bool vfs_readdir(vfs_node_t *node, uint32_t idx, vfs_dirent_t *out);

/** Eintrag in Verzeichnis suchen. */
vfs_node_t *vfs_finddir(vfs_node_t *node, const char *name);

/** Alle Mounts auf Serial ausgeben. */
void vfs_dump(void);

/* ── Registrierte Dateisysteme ───────────────────────────────────── */
extern fs_ops_t ramfs_ops;
extern fs_ops_t fat32_ops;
extern fs_ops_t ext2_ops;
extern fs_ops_t devfs_ops;
extern fs_ops_t procfs_ops;
extern fs_ops_t iso9660_ops;

#endif /* _VFS_H */
