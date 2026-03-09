/**
 * @file initrd.h
 * @brief Initial RAM-Disk (Initrd) für PinguinOS – Feature #48.
 *
 * Eine einfache CPIO-ähnliche RAM-Disk die als Multiboot-Modul
 * geladen wird.  Dateien werden aus dem Modul-Puffer entpackt
 * und ins VFS (unter /initrd) eingehängt.
 *
 * Format: PinguinOS eigenes PIRD-Format (PinguinOS InitRD)
 *   [PIRD_MAGIC 4 Byte][Anzahl Dateien 4 Byte]
 *   Für jede Datei: [Name 64 Byte][Größe 4 Byte][Daten...]
 */
#ifndef _INITRD_H
#define _INITRD_H

#include "types.h"
#include "vfs.h"   /* Fix: fs_ops_t muss hier sichtbar sein */

#define INITRD_MAGIC      0x50495244   /* "PIRD" */
#define INITRD_NAME_LEN   64
#define INITRD_MAX_FILES  64

/** Initrd aus Multiboot-Modul-Adresse initialisieren. */
void initrd_init(uint32_t mods_addr);

/** Zeiger auf Datei-Daten zurückgeben (NULL wenn nicht gefunden). */
const uint8_t *initrd_get_file(const char *name, uint32_t *size_out);

/** VFS-Operationstabelle des Initrd-Dateisystems. */
extern fs_ops_t initrd_ops;

#endif /* _INITRD_H */