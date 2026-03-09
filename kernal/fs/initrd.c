/**
 * @file initrd.c
 * @brief Initial RAM-Disk für PinguinOS – Feature #48.
 *
 * Liest ein PIRD-Archiv das von GRUB als Multiboot-Modul geladen wird.
 *
 * PIRD-Format (PinguinOS InitRD):
 *   Offset  0: uint32_t magic   = 0x50495244 ("PIRD")
 *   Offset  4: uint32_t anzahl  = Anzahl der Dateien
 *   Für jede Datei:
 *     [64 Byte Name][4 Byte Größe][N Byte Daten]
 */

#include "../include/initrd.h"
#include "../include/multiboot.h"
#include "../include/vfs.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Interne Datei-Tabelle ────────────────────────────────────── */
typedef struct {
    char      name[INITRD_NAME_LEN];
    uint8_t  *daten;     /* Zeiger in den Modul-Puffer (kein eigener Speicher) */
    uint32_t  groesse;
} initrd_datei_t;

static initrd_datei_t dateien[INITRD_MAX_FILES];
static uint32_t       datei_count = 0;
static bool           initrd_ok   = false;

/* ── Öffentlich: initrd_init ─────────────────────────────────── */
void initrd_init(uint32_t mods_addr)
{
    /* Multiboot-Modulzeiger lesen */
    if (!mods_addr) return;
    multiboot_module_t *mod = (multiboot_module_t *)mods_addr;

    /* Mindestgröße prüfen */
    if (mod->mod_end <= mod->mod_start + 8) return;

    uint8_t  *daten  = (uint8_t *)mod->mod_start;
    uint32_t  magic  = *(uint32_t *)(daten);
    uint32_t  anzahl = *(uint32_t *)(daten + 4);

    /* Magic prüfen */
    if (magic != INITRD_MAGIC) {
        KERR("Initrd: ungültiges Magic 0x%08x (erwartet 0x%08x)\n",
             magic, INITRD_MAGIC);
        return;
    }

    if (anzahl > INITRD_MAX_FILES) anzahl = INITRD_MAX_FILES;

    /* Dateien parsen */
    uint8_t *p = daten + 8;
    uint8_t *ende = (uint8_t *)mod->mod_end;

    for (uint32_t i = 0; i < anzahl && p + INITRD_NAME_LEN + 4 <= ende; i++) {
        /* Name (64 Byte, NUL-terminiert) */
        strncpy(dateien[i].name, (char *)p, INITRD_NAME_LEN - 1);
        p += INITRD_NAME_LEN;

        /* Größe (4 Byte LE) */
        dateien[i].groesse = *(uint32_t *)p;
        p += 4;

        /* Daten-Zeiger (direkt in den Modulspeicher) */
        if (p + dateien[i].groesse > ende) {
            KERR("Initrd: Datei '%s' überläuft Modul-Puffer\n", dateien[i].name);
            break;
        }
        dateien[i].daten = p;
        p += dateien[i].groesse;
        datei_count++;

        KINFO("Initrd: Datei '%s'  %u Byte\n",
              dateien[i].name, dateien[i].groesse);
    }

    initrd_ok = true;
    KINFO("Initrd: %u Datei(en) geladen\n", datei_count);
}

/* ── Öffentlich: initrd_get_file ─────────────────────────────── */
const uint8_t *initrd_get_file(const char *name, uint32_t *size_out)
{
    for (uint32_t i = 0; i < datei_count; i++) {
        if (strcmp(dateien[i].name, name) == 0) {
            if (size_out) *size_out = dateien[i].groesse;
            return dateien[i].daten;
        }
    }
    return NULL;
}

/* ── VFS-Implementierung ─────────────────────────────────────── */
static int32_t initrd_read(vfs_node_t *node, uint32_t offset,
                            uint32_t size, uint8_t *buf)
{
    /* node->inode_num = Index in dateien[] */
    uint32_t idx = node->inode_num;
    if (idx >= datei_count) return -1;
    if (offset >= dateien[idx].groesse) return 0;

    uint32_t rest = dateien[idx].groesse - offset;
    uint32_t kopieren = rest < size ? rest : size;
    memcpy(buf, dateien[idx].daten + offset, kopieren);
    return (int32_t)kopieren;
}

static bool initrd_readdir(vfs_node_t *node, uint32_t index, vfs_dirent_t *out)
{
    (void)node;
    if (index >= datei_count) return false;
    strncpy(out->name, dateien[index].name, 255);
    out->inode_num = index;
    out->type      = VFS_FILE;
    return true;
}

static vfs_node_t *initrd_finddir(vfs_node_t *parent, const char *name)
{
    (void)parent;
    for (uint32_t i = 0; i < datei_count; i++) {
        if (strcmp(dateien[i].name, name) != 0) continue;
        vfs_node_t *n = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
        if (!n) return NULL;
        strncpy(n->name, dateien[i].name, 255);
        n->size      = dateien[i].groesse;
        n->inode_num = i;
        n->flags     = VFS_FILE;
        n->ops       = &initrd_ops;
        return n;
    }
    return NULL;
}

static vfs_node_t *initrd_mount(const char *device, uint32_t flags)
{
    (void)device; (void)flags;
    if (!initrd_ok) return NULL;
    vfs_node_t *wurzel = (vfs_node_t *)kzalloc(sizeof(vfs_node_t));
    if (!wurzel) return NULL;
    memcpy(wurzel->name, "/", 2);
    wurzel->flags = VFS_DIR;
    wurzel->ops   = &initrd_ops;
    return wurzel;
}

fs_ops_t initrd_ops = {
    .name    = "initrd",
    .read    = initrd_read,
    .write   = NULL,
    .readdir = initrd_readdir,
    .finddir = initrd_finddir,
    .mount   = initrd_mount,
};