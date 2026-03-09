/**
 * @file ac97.h
 * @brief AC97-Audio-Treiber für PinguinOS – Features #46–47.
 *
 * Implementiert den Intel Audio Codec 97 (AC97):
 *   #46 – AC97-Codec-Erkennung und -Initialisierung
 *   #47 – PCM-Ausgabe über DMA-Buffer-Descriptor-Liste (BDL)
 *
 * PCI-Kennung:  Klasse 0x04, Unterklasse 0x01 (Multimedia Audio-Controller)
 *              Alternativ: Klasse 0x04, Unterklasse 0x03 (ICH AC97)
 *
 * Register-Aufteilung:
 *   – BAR0: Native Audio Mixer (NAM) – Codec-Steuerung
 *   – BAR1: Native Audio Bus Master (NABM) – DMA-Steuerung
 */
#ifndef _AC97_H
#define _AC97_H

#include "types.h"

/* ── Maximale Anzahl DMA-Puffer ──────────────────────────────────── */
#define AC97_BDL_SIZE      32    /* Einträge in der Buffer-Descriptor-Liste */
#define AC97_BUF_SAMPLES  1024   /* Samples pro DMA-Puffer                 */
#define AC97_SAMPLE_RATE  44100  /* Standard-Abtastrate in Hz              */

/* ── NAM-Register-Offsets (Mixer, BAR0 I/O) ─────────────────────── */
#define AC97_NAM_RESET       0x00   /* Codec-Reset                        */
#define AC97_NAM_MASTER_VOL  0x02   /* Master-Lautstärke                  */
#define AC97_NAM_PCM_VOL     0x18   /* PCM-Ausgabe-Lautstärke             */
#define AC97_NAM_SAMPLE_RATE 0x2C   /* PCM-Front-DAC-Rate                 */
#define AC97_NAM_EXT_AUDIO   0x28   /* Extended Audio ID                  */
#define AC97_NAM_EXT_CTRL    0x2A   /* Extended Audio Status/Control      */

/* ── NABM-Register-Offsets (Bus-Master, BAR1 I/O) ───────────────── */
#define AC97_NABM_POBDBAR  0x10   /* PCM-Out BDL-Basisadresse            */
#define AC97_NABM_POCIV    0x14   /* PCM-Out aktueller Index             */
#define AC97_NABM_POLVI    0x15   /* PCM-Out letzter gültiger Index      */
#define AC97_NABM_POSR     0x16   /* PCM-Out Statusregister              */
#define AC97_NABM_POPICB   0x18   /* PCM-Out Samples verbleibend         */
#define AC97_NABM_POPIVCR  0x1B   /* PCM-Out Steuerregister              */

/* ── PCM-Out-Steuerregister-Bits ─────────────────────────────────── */
#define AC97_CTRL_RUN    (1u << 0)  /* DMA-Transfer starten/stoppen      */
#define AC97_CTRL_RESET  (1u << 1)  /* Kanal-Reset                       */
#define AC97_CTRL_LVBIE  (1u << 2)  /* Interrupt bei Last-Valid-Buffer    */
#define AC97_CTRL_IOCE   (1u << 4)  /* IOC-Interrupt aktivieren           */

/* ── Statusregister-Bits ─────────────────────────────────────────── */
#define AC97_SR_DCH   (1u << 0)   /* DMA-Controller angehalten          */
#define AC97_SR_CELV  (1u << 1)   /* Aktueller == letzter gültiger Index */
#define AC97_SR_LVBCI (1u << 2)   /* Last-Valid-Buffer-Interrupt         */
#define AC97_SR_BCIS  (1u << 3)   /* Buffer-Completion-Interrupt         */
#define AC97_SR_FIFOE (1u << 4)   /* FIFO-Fehler                        */

/* ── Buffer-Descriptor-Eintrag (8 Byte) ─────────────────────────── */
typedef struct PACKED {
    uint32_t addr;    /* Physische Pufferadresse                      */
    uint16_t samples; /* Anzahl Samples in diesem Puffer              */
    uint16_t flags;   /* BUP (Buffer Underrun Policy), IOC            */
} ac97_bde_t;

/* ── BDE-Flags ────────────────────────────────────────────────────── */
#define AC97_BDE_IOC   (1u << 15)  /* Interrupt bei Puffer-Ende         */
#define AC97_BDE_BUP   (1u << 14)  /* Letzten Sample bei Underrun halten */

/* ── Audio-Format ────────────────────────────────────────────────── */
typedef enum {
    AC97_FMT_S16_STEREO = 0,  /* 16-Bit Stereo (Standard)             */
    AC97_FMT_S16_MONO   = 1,  /* 16-Bit Mono                          */
} ac97_format_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief AC97-Treiber initialisieren.
 *
 * Sucht einen AC97-Audio-Controller via PCI, setzt den Codec zurück,
 * konfiguriert Lautstärke und Abtastrate und richtet die BDL ein.
 * @return true bei Erfolg, false wenn kein Gerät gefunden.
 */
bool ac97_init(void);

/**
 * @brief PCM-Samples zur Ausgabe einreihen.
 *
 * @param samples  Zeiger auf 16-Bit-Stereo-PCM-Daten.
 * @param count    Anzahl der Samples (Paare bei Stereo).
 * @return 0 bei Erfolg, -1 bei vollem Puffer.
 */
int ac97_play(const int16_t *samples, uint32_t count);

/**
 * @brief Abtastrate ändern (nur wenn VRA-Bit unterstützt).
 *
 * @param hz  Neue Abtastrate in Hz (z. B. 44100, 48000).
 * @return 0 bei Erfolg, -1 wenn VRA nicht unterstützt.
 */
int ac97_set_sample_rate(uint32_t hz);

/** PCM-Out-DMA anhalten. */
void ac97_stop(void);

/** PCM-Out-DMA fortsetzen. */
void ac97_resume(void);

/** Master-Lautstärke setzen (0 = max, 31 = min, 32 = stumm). */
void ac97_set_volume(uint8_t vol);

/** @return true wenn AC97-Treiber initialisiert und bereit. */
bool ac97_ready(void);

/** AC97-Status auf seriell ausgeben. */
void ac97_dump(void);

#endif /* _AC97_H */