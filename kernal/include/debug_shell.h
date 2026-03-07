/**
 * @file debug_shell.h + debug_shell.c
 * @brief Interaktive Kernel-Debug-Shell über COM1 (Feature #91).
 *
 * Befehle:
 *   help       – Befehlsliste
 *   mem        – Speicherinfo (PMM, Heap, SLAB)
 *   ps         – Prozess-/Task-Liste
 *   pci        – PCI-Geräteliste
 *   vfs        – VFS-Mounts
 *   net        – Netzwerk-Statistiken
 *   arp        – ARP-Cache
 *   reboot     – System neu starten
 *   shutdown   – System herunterfahren
 *   echo <txt> – Text ausgeben
 *   hexdump <addr> <len> – Speicher-Dump
 *   mmap <virt>          – physische Adresse anzeigen
 *   uptime               – Betriebszeit
 *   panic                – Kernel-Panic auslösen (Test)
 */
#ifndef _DEBUG_SHELL_H
#define _DEBUG_SHELL_H

#include "types.h"

/** Startet die Debug-Shell als Kernel-Task (blockiert nicht). */
void debug_shell_init(void);

/** Direkt in der Shell-Schleife bleiben (blockierend). */
void debug_shell_run(void);

#endif /* _DEBUG_SHELL_H */
