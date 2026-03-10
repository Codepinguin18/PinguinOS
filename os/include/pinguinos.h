/**
 * @file pinguinos.h
 * @brief PinguinOS Desktop-Umgebung – Einstiegspunkt.
 *
 * Wird vom Kernel-Einstiegspunkt (kernel.c) nach vollständiger
 * Hardware-Initialisierung aufgerufen.
 */
#ifndef _PINGUINOS_H
#define _PINGUINOS_H

/** PinguinOS starten (blockiert dauerhaft – Kernel-Hauptschleife). */
void pinguinos_main(void);

#endif /* _PINGUINOS_H */