/**
 * @file ac97.c
 * @brief AC97-Audio-Treiber für PinguinOS – Features #46–47.
 *
 * Vollständige Implementierung:
 *   – PCI-Enumeration (Klasse 0x04, Unterklasse 0x01 / 0x03)
 *   – Codec-Reset über NAM-Register
 *   – Lautstärke- und Abtastrate-Konfiguration
 *   – DMA-Buffer-Descriptor-Liste (BDL) für PCM-Out
 *   – Ring-Puffer mit AC97_BDL_SIZE Einträgen
 */

#include "../include/ac97.h"
#include "../include/pci.h"
#include "../include/cpu.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Modul-Zustand ───────────────────────────────────────────────── */
static bool     ac97_ok      = false;
static uint16_t nam_base     = 0;   /* Native Audio Mixer I/O-Basis   */
static uint16_t nabm_base    = 0;   /* Native Audio Bus Master I/O-Basis */
static uint32_t sample_rate  = AC97_SAMPLE_RATE;

/* ── DMA-Puffer ──────────────────────────────────────────────────── */
static ac97_bde_t  bdl[AC97_BDL_SIZE];              /* Buffer-Descriptor-Liste  */
static int16_t    *pcm_bufs[AC97_BDL_SIZE];          /* PCM-Sample-Puffer        */
static uint32_t    bdl_write_idx = 0;               /* Schreib-Index (Software) */

/* ── I/O-Hilfsfunktionen (NAM) ───────────────────────────────────── */
static INLINE uint16_t nam_inw(uint16_t reg)
{
    return inw((uint16_t)(nam_base + reg));
}
static INLINE void nam_outw(uint16_t reg, uint16_t v)
{
    outw((uint16_t)(nam_base + reg), v);
}

/* ── I/O-Hilfsfunktionen (NABM) ──────────────────────────────────── */
static INLINE uint8_t nabm_inb(uint16_t reg)
{
    return inb((uint16_t)(nabm_base + reg));
}
static INLINE void nabm_outb(uint16_t reg, uint8_t v)
{
    outb((uint16_t)(nabm_base + reg), v);
}
static INLINE void nabm_outl(uint16_t reg, uint32_t v)
{
    outl((uint16_t)(nabm_base + reg), v);
}

/* ── Kurze Verzögerung ───────────────────────────────────────────── */
static void ac97_delay(uint32_t n)
{
    for (uint32_t i = 0; i < n * 1000; i++)
        __asm__ volatile ("nop");
}

/* ── Codec auf Antwort warten ────────────────────────────────────── */
static bool ac97_wait_codec(void)
{
    /* Nach Reset wartet der Codec bis zu 500 ms (AC97-Spezifikation) */
    uint32_t timeout = 5000000;
    while (timeout--) {
        /* Wenn das Primär-Codec bereit ist, gibt READ/WRITE keine Fehler */
        uint16_t v = nam_inw(AC97_NAM_RESET);
        if (v != 0xFFFF) return true;
        __asm__ volatile ("nop");
    }
    return false;
}

/* ── Öffentlich: ac97_set_volume ─────────────────────────────────── */
void ac97_set_volume(uint8_t vol)
{
    if (!ac97_ok) return;
    /* Master-Lautstärke: Bit 15 = Mute, Bits 12:8 = rechts, Bits 4:0 = links */
    if (vol >= 32) {
        nam_outw(AC97_NAM_MASTER_VOL, 0x8000);  /* Stummschalten */
    } else {
        uint16_t v = (uint16_t)((vol & 0x1F) | ((vol & 0x1F) << 8));
        nam_outw(AC97_NAM_MASTER_VOL, v);
    }
}

/* ── Öffentlich: ac97_set_sample_rate ────────────────────────────── */
int ac97_set_sample_rate(uint32_t hz)
{
    if (!ac97_ok) return -1;

    /* Extended-Audio-Statusregister prüfen: VRA (Variable Rate Audio) */
    uint16_t ext = nam_inw(AC97_NAM_EXT_AUDIO);
    if (!(ext & 0x01)) {
        /* VRA nicht unterstützt: Codec arbeitet nur mit 48 kHz */
        KWARN("AC97: VRA nicht unterstützt – Abtastrate bleibt 48000 Hz\n");
        return -1;
    }

    /* VRA im Extended-Audio-Control-Register aktivieren */
    nam_outw(AC97_NAM_EXT_CTRL,
             nam_inw(AC97_NAM_EXT_CTRL) | 0x01);

    /* Neue Rate setzen */
    nam_outw(AC97_NAM_SAMPLE_RATE, (uint16_t)hz);
    /* Zurücklesen um tatsächliche Rate zu prüfen */
    uint16_t actual = nam_inw(AC97_NAM_SAMPLE_RATE);
    sample_rate = actual;
    KINFO("AC97: Abtastrate gesetzt auf %u Hz (angefragt: %u)\n",
          actual, hz);
    return 0;
}

/* ── Öffentlich: ac97_stop ───────────────────────────────────────── */
void ac97_stop(void)
{
    if (!ac97_ok) return;
    /* DMA-Transfer anhalten: RUN-Bit löschen */
    nabm_outb(AC97_NABM_POPIVCR,
              nabm_inb(AC97_NABM_POPIVCR) & ~AC97_CTRL_RUN);
}

/* ── Öffentlich: ac97_resume ─────────────────────────────────────── */
void ac97_resume(void)
{
    if (!ac97_ok) return;
    nabm_outb(AC97_NABM_POPIVCR,
              nabm_inb(AC97_NABM_POPIVCR) | AC97_CTRL_RUN);
}

/* ── Öffentlich: ac97_play ───────────────────────────────────────── */
int ac97_play(const int16_t *samples, uint32_t count)
{
    if (!ac97_ok || !samples || !count) return -1;

    uint32_t idx = bdl_write_idx % AC97_BDL_SIZE;

    /* Samples in DMA-Puffer kopieren (kürzen falls nötig) */
    uint32_t copy = count;
    if (copy > AC97_BUF_SAMPLES * 2)
        copy = AC97_BUF_SAMPLES * 2;  /* Stereo: 2× samples pro Eintrag */
    memcpy(pcm_bufs[idx], samples, copy * sizeof(int16_t));

    /* BDE aktualisieren */
    bdl[idx].samples = (uint16_t)copy;
    bdl[idx].flags   = AC97_BDE_IOC;

    /* Letzten gültigen Index voranstellen */
    nabm_outb(AC97_NABM_POLVI, (uint8_t)idx);

    /* DMA starten, falls noch nicht laufend */
    if (!(nabm_inb(AC97_NABM_POPIVCR) & AC97_CTRL_RUN))
        nabm_outb(AC97_NABM_POPIVCR, AC97_CTRL_RUN | AC97_CTRL_IOCE);

    bdl_write_idx = (bdl_write_idx + 1) % AC97_BDL_SIZE;
    return 0;
}

/* ── Öffentlich: ac97_init ───────────────────────────────────────── */
bool ac97_init(void)
{
    /* PCI: Multimedia-Audio-Controller suchen */
    pci_device_t *pci_dev = pci_find_class(0x04, 0x01);
    if (!pci_dev) pci_dev = pci_find_class(0x04, 0x03);
    if (!pci_dev) {
        KINFO("AC97: Kein Audio-Controller gefunden\n");
        return false;
    }

    /* I/O-Basisadressen aus BAR0 (NAM) und BAR1 (NABM) */
    nam_base  = (uint16_t)(pci_dev->bar[0] & ~0x3u);
    nabm_base = (uint16_t)(pci_dev->bar[1] & ~0x3u);
    if (!nam_base || !nabm_base) {
        KERR("AC97: Ungültige BARs (NAM=0x%04x NABM=0x%04x)\n",
             nam_base, nabm_base);
        return false;
    }

    pci_enable_bus_master(pci_dev);

    /* PCM-Out-Kanal zurücksetzen */
    nabm_outb(AC97_NABM_POPIVCR, AC97_CTRL_RESET);
    ac97_delay(100);
    nabm_outb(AC97_NABM_POPIVCR, 0);

    /* Cold-Reset des Codecs über NAM-Reset-Register */
    nam_outw(AC97_NAM_RESET, 0x0000);
    ac97_delay(500);

    /* Warten bis Codec antwortet */
    if (!ac97_wait_codec()) {
        KERR("AC97: Codec antwortet nicht\n");
        return false;
    }

    /* PCM-Ausgabe-Lautstärke auf Maximum (0 = kein Dämpfen) */
    nam_outw(AC97_NAM_PCM_VOL,    0x0000);
    /* Master-Lautstärke: 0 dB (kein Mute) */
    nam_outw(AC97_NAM_MASTER_VOL, 0x0000);

    /* Abtastrate auf 44100 Hz setzen (VRA falls unterstützt) */
    uint16_t ext = nam_inw(AC97_NAM_EXT_AUDIO);
    if (ext & 0x01) {
        nam_outw(AC97_NAM_EXT_CTRL,
                 nam_inw(AC97_NAM_EXT_CTRL) | 0x01);
        nam_outw(AC97_NAM_SAMPLE_RATE, (uint16_t)AC97_SAMPLE_RATE);
        sample_rate = nam_inw(AC97_NAM_SAMPLE_RATE);
    } else {
        sample_rate = 48000;  /* Codec-Standard ohne VRA */
    }

    /* DMA-Puffer allozieren */
    for (int i = 0; i < AC97_BDL_SIZE; i++) {
        pcm_bufs[i] = (int16_t *)kmalloc(AC97_BUF_SAMPLES * 2 * sizeof(int16_t));
        if (!pcm_bufs[i]) { KERR("AC97: Kein Speicher für Puffer %d\n", i); return false; }
        memset(pcm_bufs[i], 0, AC97_BUF_SAMPLES * 2 * sizeof(int16_t));

        bdl[i].addr    = (uint32_t)pcm_bufs[i];
        bdl[i].samples = AC97_BUF_SAMPLES * 2;  /* Stereo */
        bdl[i].flags   = AC97_BDE_IOC;
    }

    /* BDL-Basisadresse in NABM schreiben */
    nabm_outl(AC97_NABM_POBDBAR, (uint32_t)bdl);
    /* Letzten gültigen Index auf 0 setzen */
    nabm_outb(AC97_NABM_POLVI, 0);

    ac97_ok = true;
    KINFO("AC97: bereit  NAM=0x%04x NABM=0x%04x  %u Hz\n",
          nam_base, nabm_base, sample_rate);
    return true;
}

bool ac97_ready(void) { return ac97_ok; }

void ac97_dump(void)
{
    if (!ac97_ok) { KINFO("AC97: nicht initialisiert\n"); return; }
    KINFO("AC97: NAM=0x%04x  NABM=0x%04x  Rate=%u Hz\n",
          nam_base, nabm_base, sample_rate);
    KINFO("AC97: BDL-Schreibindex=%u\n", bdl_write_idx);
}