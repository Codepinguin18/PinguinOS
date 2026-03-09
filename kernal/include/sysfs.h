/**
 * @file sysfs.h
 * @brief SysFS – virtuelles Gerätedateisystem – Feature #54.
 *
 * SysFS stellt Kernel-interne Zustandsvariablen als virtuelle
 * Dateien unter /sys bereit.  Treiber können Einträge
 * mit sysfs_register() registrieren.
 */
#ifndef _SYSFS_H
#define _SYSFS_H

#include "types.h"
#include "vfs.h"

#define SYSFS_ENTRIES_MAX  64
#define SYSFS_NAME_LEN     32
#define SYSFS_VAL_LEN      256

/**
 * @brief Lese-Callback: Wert als String in buf schreiben.
 * @return Anzahl geschriebener Bytes (ohne NUL).
 */
typedef int (*sysfs_read_fn)(char *buf, uint32_t maxlen);

/**
 * @brief Schreib-Callback: Wert aus String setzen.
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
typedef int (*sysfs_write_fn)(const char *buf, uint32_t len);

/** Einen SysFS-Eintrag registrieren. */
int sysfs_register(const char *name, sysfs_read_fn rd, sysfs_write_fn wr);

/** Einen SysFS-Eintrag entfernen. */
void sysfs_unregister(const char *name);

/** SysFS-fs_ops für den VFS-Layer. */
extern fs_ops_t sysfs_ops;

#endif /* _SYSFS_H */