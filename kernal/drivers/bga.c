/**
 * @file bga.c
 * @brief Bochs Graphics Adapter (BGA/VBE) driver for PinguinOS.
 *
 * Feature #48 – BGA Graphics.
 *
 * The BGA exposes a linear framebuffer at a fixed physical address
 * (0xE0000000 in QEMU) controlled through two I/O ports:
 *   0x01CE – VBE index register
 *   0x01CF – VBE data register
 *
 * After bga_set_mode() the framebuffer is identity-mapped into the
 * kernel address space via paging_map().
 */

#include "../include/bga.h"
#include "../include/paging.h"
#include "../include/mm.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/pci.h"

/* ── BGA I/O registers ───────────────────────────────────────────── */
#define BGA_IOPORT_INDEX  0x01CE
#define BGA_IOPORT_DATA   0x01CF

/* ── BGA VBE register indices ────────────────────────────────────── */
#define VBE_DISPI_INDEX_ID           0x00
#define VBE_DISPI_INDEX_XRES         0x01
#define VBE_DISPI_INDEX_YRES         0x02
#define VBE_DISPI_INDEX_BPP          0x03
#define VBE_DISPI_INDEX_ENABLE       0x04
#define VBE_DISPI_INDEX_BANK         0x05
#define VBE_DISPI_INDEX_VIRT_WIDTH   0x06
#define VBE_DISPI_INDEX_VIRT_HEIGHT  0x07
#define VBE_DISPI_INDEX_X_OFFSET     0x08
#define VBE_DISPI_INDEX_Y_OFFSET     0x09

/* ── BGA enable flags ────────────────────────────────────────────── */
#define VBE_DISPI_DISABLED         0x00
#define VBE_DISPI_ENABLED          0x01
#define VBE_DISPI_LFB_ENABLED      0x40   /* Linear framebuffer */
#define VBE_DISPI_NOCLEARMEM       0x80

/* Physical address of the BGA linear framebuffer – probed from PCI BAR 0 */
/* Fallback to QEMU default if PCI probe fails */
#define BGA_FRAMEBUFFER_PHYS_DEFAULT  0xE0000000u

static uint32_t bga_fb_phys = BGA_FRAMEBUFFER_PHYS_DEFAULT;  /* set by bga_probe_bar() */
#define BGA_FRAMEBUFFER_VIRT  bga_fb_phys   /* identity-mapped */

/* Maximum framebuffer size to map: 8 MB */
#define BGA_FB_MAP_SIZE  (8u * 1024u * 1024u)

/* ── Module state ────────────────────────────────────────────────── */
static uint16_t  bga_width   = 0;
static uint16_t  bga_height  = 0;
static uint16_t  bga_bpp_val = 0;
static bool      bga_active  = false;
static void     *fb_ptr      = NULL;

/* ── PCI BAR probe ───────────────────────────────────────────────── */
/**
 * Finds the VGA/display PCI device (class 0x03) and reads BAR 0 to get
 * the actual linear framebuffer physical address.  Falls back to the
 * QEMU default (0xE0000000) if no device is found.
 */
static void bga_probe_bar(void)
{
    /* Try VGA-compatible (class=0x03, sub=0x00) then any display device */
    pci_device_t *vga = pci_find_class(PCI_CLASS_DISPLAY, 0x00);
    if (!vga) vga = pci_find_class(PCI_CLASS_DISPLAY, 0x80);
    if (!vga) {
        KINFO("BGA: PCI VGA device not found – using default 0x%08x\n",
              BGA_FRAMEBUFFER_PHYS_DEFAULT);
        bga_fb_phys = BGA_FRAMEBUFFER_PHYS_DEFAULT;
        return;
    }

    /* BAR 0: mask out type/flag bits for memory BAR */
    uint32_t bar0 = vga->bar[0];
    uint32_t addr = bar0 & 0xFFFFFFF0u;   /* mask low 4 bits (type/flags) */

    KINFO("BGA: PCI VGA %04x:%04x at %02x:%02x.%x  BAR0=0x%08x  addr=0x%08x\n",
          vga->vendor_id, vga->device_id,
          vga->bus, vga->device, vga->function,
          bar0, addr);

    if (addr == 0 || addr == 0xFFFFFFF0u) {
        /* BAR not configured yet – use default */
        bga_fb_phys = BGA_FRAMEBUFFER_PHYS_DEFAULT;
        KINFO("BGA: BAR0 unconfigured – using default 0x%08x\n",
              BGA_FRAMEBUFFER_PHYS_DEFAULT);
    } else {
        bga_fb_phys = addr;
    }

    /* Ensure the framebuffer region is accessible – map it if needed.
     * paging_map_page() silently ignores PSE-already-mapped regions. */
    uint32_t pages = (BGA_FB_MAP_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        uint32_t off = i * PAGE_SIZE;
        paging_map_page(bga_fb_phys + off, bga_fb_phys + off,
                        PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);
    }

    KINFO("BGA: framebuffer mapped at phys=0x%08x (%u pages)\n",
          bga_fb_phys, pages);
}


static void bga_write(uint16_t idx, uint16_t val)
{
    outw(BGA_IOPORT_INDEX, idx);
    outw(BGA_IOPORT_DATA,  val);
}

static uint16_t bga_read(uint16_t idx)
{
    outw(BGA_IOPORT_INDEX, idx);
    return inw(BGA_IOPORT_DATA);
}

/* ── Public: bga_detect ──────────────────────────────────────────── */
bool bga_detect(void)
{
    uint16_t id = bga_read(VBE_DISPI_INDEX_ID);
    /* Valid IDs: 0xB0C0 .. 0xB0C5 */
    bool ok = (id >= 0xB0C0 && id <= 0xB0C5);
    if (ok)
        KINFO("BGA: detected  ID=0x%04x\n", id);
    else
        KINFO("BGA: not present (ID=0x%04x)\n", id);
    return ok;
}

/* ── Public: bga_set_mode ────────────────────────────────────────── */
bool bga_set_mode(uint16_t width, uint16_t height, uint16_t bpp)
{
    if (!bga_detect()) return false;

    /* Only 32 bpp supported by this driver for simplicity */
    if (bpp != 32 && bpp != 24 && bpp != 16 && bpp != 8) return false;

    /* Probe the real framebuffer address from PCI BAR 0 */
    bga_probe_bar();

    /* Disable VBE before changing parameters */
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    bga_write(VBE_DISPI_INDEX_XRES, width);
    bga_write(VBE_DISPI_INDEX_YRES, height);
    bga_write(VBE_DISPI_INDEX_BPP,  bpp);

    /* Enable with linear framebuffer, don't clear memory */
    bga_write(VBE_DISPI_INDEX_ENABLE,
              VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED | VBE_DISPI_NOCLEARMEM);

    bga_width   = width;
    bga_height  = height;
    bga_bpp_val = bpp;
    bga_active  = true;

    fb_ptr = (void *)bga_fb_phys;

    KINFO("BGA: mode set  %ux%u @ %ubpp  fb=0x%08x\n",
          width, height, bpp, bga_fb_phys);
    return true;
}

/* ── Public: bga_disable ─────────────────────────────────────────── */
void bga_disable(void)
{
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_active = false;
    fb_ptr = NULL;
    KINFO("BGA: disabled\n");
}

/* ── Public: accessors ───────────────────────────────────────────── */
void    *bga_get_framebuffer(void) { return bga_active ? fb_ptr : NULL; }
uint32_t bga_get_pitch(void)       { return bga_active ? (uint32_t)bga_width * (bga_bpp_val / 8) : 0; }
uint16_t bga_get_width(void)       { return bga_width;   }
uint16_t bga_get_height(void)      { return bga_height;  }
uint16_t bga_get_bpp(void)         { return bga_bpp_val; }

/* ── Public: pixel operations ────────────────────────────────────── */
void bga_put_pixel(uint16_t x, uint16_t y, uint32_t colour)
{
    if (!bga_active || !fb_ptr) return;
    uint32_t *fb = (uint32_t *)fb_ptr;
    fb[(uint32_t)y * bga_width + x] = colour;
}

void bga_clear(uint32_t colour)
{
    if (!bga_active || !fb_ptr) return;
    uint32_t *fb    = (uint32_t *)fb_ptr;
    uint32_t  total = (uint32_t)bga_width * bga_height;
    for (uint32_t i = 0; i < total; i++)
        fb[i] = colour;
}

void bga_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                   uint32_t colour)
{
    if (!bga_active || !fb_ptr) return;
    uint32_t *fb = (uint32_t *)fb_ptr;
    for (uint16_t row = y; row < y + h && row < bga_height; row++) {
        for (uint16_t col = x; col < x + w && col < bga_width; col++) {
            fb[(uint32_t)row * bga_width + col] = colour;
        }
    }
}