/**
 * @file rtc.c
 * @brief RTC-Treiber (CMOS) + HPET für PinguinOS.
 */
#include "../include/rtc.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/paging.h"

/* ── CMOS-Hilfsfunktionen ────────────────────────────────────────── */
static uint8_t cmos_read(uint8_t reg)
{
    outb(CMOS_ADDR, reg & 0x7F);   /* Bit 7 = NMI-Disable-Flag */
    return inb(CMOS_DATA);
}

static bool cmos_updating(void)
{
    return (cmos_read(RTC_REG_STATUSA) & 0x80) != 0;
}

static uint8_t bcd_to_bin(uint8_t bcd)
{
    return (uint8_t)(((bcd >> 4) * 10) + (bcd & 0x0F));
}

/* ── Öffentlich: rtc_init ────────────────────────────────────────── */
void rtc_init(void)
{
    /* Sicherstellen dass 24h-Format und BCD aktiv */
    KINFO("RTC: CMOS-Treiber initialisiert\n");
}

/* ── Öffentlich: rtc_read ────────────────────────────────────────── */
void rtc_read(rtc_time_t *t)
{
    /* Warten bis keine Update-in-Progress-Condition */
    while (cmos_updating());

    uint8_t sec, min, hour, day, month, year, wday;
    uint8_t statusb;

    /* Zweimal lesen um inkonsistente Werte zu erkennen */
    do {
        sec   = cmos_read(RTC_REG_SEC);
        min   = cmos_read(RTC_REG_MIN);
        hour  = cmos_read(RTC_REG_HOUR);
        day   = cmos_read(RTC_REG_MDAY);
        month = cmos_read(RTC_REG_MONTH);
        year  = cmos_read(RTC_REG_YEAR);
        wday  = cmos_read(RTC_REG_WDAY);
        while (cmos_updating());
    } while (sec != cmos_read(RTC_REG_SEC));

    statusb = cmos_read(RTC_REG_STATUSB);

    /* BCD → Binär konvertieren (falls BCD-Modus aktiv) */
    if (!(statusb & 0x04)) {
        sec   = bcd_to_bin(sec);
        min   = bcd_to_bin(min);
        hour  = bcd_to_bin(hour);
        day   = bcd_to_bin(day);
        month = bcd_to_bin(month);
        year  = bcd_to_bin(year);
        wday  = bcd_to_bin(wday);
    }

    /* 12h → 24h */
    if (!(statusb & 0x02) && (hour & 0x80)) {
        hour = (uint8_t)(((hour & 0x7F) + 12) % 24);
    }

    t->second  = sec;
    t->minute  = min;
    t->hour    = hour;
    t->day     = day;
    t->month   = month;
    t->year    = (uint16_t)(2000 + year);
    t->weekday = wday;
}

void rtc_dump(void)
{
    rtc_time_t t;
    rtc_read(&t);
    KINFO("RTC: %04u-%02u-%02u %02u:%02u:%02u\n",
          t.year, t.month, t.day, t.hour, t.minute, t.second);
}

uint32_t rtc_to_unix(const rtc_time_t *t)
{
    /* Vereinfachte Unix-Timestamp-Berechnung (gültig ab 1970) */
    static const uint16_t days_in_month[] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };

    uint32_t year  = t->year - 1970;
    uint32_t leaps = (year + 2) / 4;
    uint32_t days  = year * 365 + leaps;
    days += days_in_month[t->month - 1];
    if (t->month > 2 && ((t->year % 4 == 0 && t->year % 100 != 0) ||
                          t->year % 400 == 0))
        days++;
    days += t->day - 1;

    return days * 86400 + t->hour * 3600 + t->minute * 60 + t->second;
}

/* ── HPET ────────────────────────────────────────────────────────── */
static volatile uint64_t *hpet_base = NULL;
static uint64_t hpet_period_fs = 0;   /* Periode in Femtosekunden */

#define HPET_REG(off) (*(volatile uint64_t *)((uint8_t *)hpet_base + (off)))

void hpet_init(uint32_t base_phys)
{
    /* HPET-MMIO in den Kernel-Adressraum mappen */
    paging_map_page(base_phys, base_phys, PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);
    hpet_base = (volatile uint64_t *)base_phys;

    uint64_t caps     = HPET_REG(HPET_REG_CAPS);
    hpet_period_fs    = caps >> 32;   /* Bits 63:32 = Counter-Clk-Period */

    if (hpet_period_fs == 0 || hpet_period_fs > 100000000ULL) {
        KWARN("HPET: ungültige Periode %llu fs – deaktiviert\n", hpet_period_fs);
        hpet_base = NULL;
        return;
    }

    /* HPET aktivieren */
    HPET_REG(HPET_REG_CONFIG) |= 1;

    uint32_t freq_hz = (uint32_t)(1000000000000000ULL / hpet_period_fs);
    KINFO("HPET: %u Hz  (Period %llu fs)\n", freq_hz, hpet_period_fs);
}

uint64_t hpet_read_counter(void)
{
    if (!hpet_base) return 0;
    return HPET_REG(HPET_REG_COUNTER);
}

uint32_t hpet_freq_hz(void)
{
    if (!hpet_period_fs) return 0;
    return (uint32_t)(1000000000000000ULL / hpet_period_fs);
}

void hpet_sleep_us(uint64_t us)
{
    if (!hpet_base || !hpet_period_fs) {
        /* Fallback: Busy-Warte-Schleife */
        for (volatile uint64_t i = 0; i < us * 100; i++) pause();
        return;
    }
    /* Ticks = us * 1_000_000_000 / period_fs */
    uint64_t ticks  = (us * 1000000000ULL) / hpet_period_fs;
    uint64_t target = hpet_read_counter() + ticks;
    while (hpet_read_counter() < target) pause();
}
