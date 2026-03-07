/**
 * @file rtc.h
 * @brief RTC/CMOS Uhr-Treiber (Feature #33) und HPET-Interface (Feature #34).
 */
#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* ── CMOS-Ports ──────────────────────────────────────────────────── */
#define CMOS_ADDR  0x70
#define CMOS_DATA  0x71

/* CMOS-Register */
#define RTC_REG_SEC    0x00
#define RTC_REG_MIN    0x02
#define RTC_REG_HOUR   0x04
#define RTC_REG_WDAY   0x06
#define RTC_REG_MDAY   0x07
#define RTC_REG_MONTH  0x08
#define RTC_REG_YEAR   0x09
#define RTC_REG_STATUSA 0x0A
#define RTC_REG_STATUSB 0x0B

/* ── Datum/Zeit-Struktur ─────────────────────────────────────────── */
typedef struct {
    uint16_t year;     /* z.B. 2025      */
    uint8_t  month;    /* 1–12           */
    uint8_t  day;      /* 1–31           */
    uint8_t  hour;     /* 0–23           */
    uint8_t  minute;   /* 0–59           */
    uint8_t  second;   /* 0–59           */
    uint8_t  weekday;  /* 1=So … 7=Sa    */
} rtc_time_t;

/* ── Öffentliche API – RTC ───────────────────────────────────────── */

/** Initialisiert den RTC-Treiber. */
void rtc_init(void);

/** Liest die aktuelle Zeit aus dem CMOS-RTC. */
void rtc_read(rtc_time_t *t);

/** Gibt Zeit auf dem seriellen Port aus. */
void rtc_dump(void);

/**
 * @brief Konvertiert rtc_time_t in Unix-Timestamp (Sekunden seit 1970).
 */
uint32_t rtc_to_unix(const rtc_time_t *t);

/* ── HPET (High Precision Event Timer) ──────────────────────────── */
/* HPET-Basisadresse wird aus ACPI HPET-Tabelle gelesen.
 * Diese Implementierung nutzt MMIO-Register direkt. */
#define HPET_BASE_DEFAULT  0xFED00000UL   /* Übliche physische Adresse */

#define HPET_REG_CAPS      0x000   /* Capabilities + ID             */
#define HPET_REG_CONFIG    0x010   /* General Configuration         */
#define HPET_REG_STATUS    0x020   /* General Interrupt Status      */
#define HPET_REG_COUNTER   0x0F0   /* Main Counter Value            */

void     hpet_init(uint32_t base_phys);
uint64_t hpet_read_counter(void);
void     hpet_sleep_us(uint64_t us);    /* Präzise Verzögerung in µs */
uint32_t hpet_freq_hz(void);

#endif /* _RTC_H */
