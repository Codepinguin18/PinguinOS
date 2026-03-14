/**
 * @file pe.c
 * @brief PE32 (Portable Executable) Loader für PinguinOS.
 *
 * Lädt PE32-Executables (i386) in den Kernel-Heap:
 *   1. MZ-Signatur prüfen
 *   2. PE-Signatur + Machine-Typ validieren
 *   3. Optional Header parsen (Entry Point, Image Base, Size)
 *   4. Sections in allokierten Speicher kopieren
 *   5. BSS-Sections mit Null füllen
 *   6. Base-Relocations anwenden (falls Image Base abweicht)
 *   7. Entry Point aufrufen
 */

#include "../include/pe.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Fehlercode-Konstanten ───────────────────────────────────────── */
#define PE_OK           0
#define PE_ERR_NULL    -1   /* Null-Zeiger übergeben               */
#define PE_ERR_SMALL   -2   /* Datei zu klein                      */
#define PE_ERR_MZ      -3   /* Kein MZ-Header                      */
#define PE_ERR_PE      -4   /* Kein PE-Signatur                    */
#define PE_ERR_ARCH    -5   /* Falsche Architektur (nicht i386)    */
#define PE_ERR_OPT     -6   /* Kein PE32 Optional-Header           */
#define PE_ERR_ALLOC   -7   /* Speicher-Allokation fehlgeschlagen  */
#define PE_ERR_SECTION -8   /* Section-Kopie fehlgeschlagen        */

/* ── Hilfsfunktionen ─────────────────────────────────────────────── */

static const char *pe_errstr(int err) __attribute__((unused));
static const char *pe_errstr(int err)
{
    switch (err) {
    case PE_OK:         return "OK";
    case PE_ERR_NULL:   return "Null-Zeiger";
    case PE_ERR_SMALL:  return "Datei zu klein";
    case PE_ERR_MZ:     return "Kein MZ-Header";
    case PE_ERR_PE:     return "Kein PE-Signatur";
    case PE_ERR_ARCH:   return "Falsche Architektur";
    case PE_ERR_OPT:    return "Kein PE32 Header";
    case PE_ERR_ALLOC:  return "Speicher voll";
    case PE_ERR_SECTION:return "Section-Fehler";
    default:            return "Unbekannt";
    }
}

/* ── Base-Relocations anwenden ───────────────────────────────────── */
static void pe_apply_relocations(uint8_t *base, uint32_t reloc_rva,
                                  uint32_t reloc_size, int32_t delta)
{
    if (delta == 0 || reloc_size == 0) return;

    uint8_t *reloc = base + reloc_rva;
    uint8_t *end   = reloc + reloc_size;

    while (reloc < end) {
        pe_base_reloc_block_t *block = (pe_base_reloc_block_t *)reloc;
        if (block->SizeOfBlock == 0) break;

        uint32_t num_entries = (block->SizeOfBlock - sizeof(*block)) / 2;
        uint16_t *entries = (uint16_t *)(reloc + sizeof(*block));

        for (uint32_t i = 0; i < num_entries; i++) {
            uint16_t type   = entries[i] >> 12;
            uint16_t offset = entries[i] & 0x0FFF;

            if (type == PE_REL_BASED_HIGHLOW) {
                uint32_t *patch = (uint32_t *)(base + block->VirtualAddress + offset);
                *patch += (uint32_t)delta;
            }
            /* PE_REL_BASED_ABSOLUTE = padding, ignorieren */
        }

        reloc += block->SizeOfBlock;
    }
}

/* ── pe_load ─────────────────────────────────────────────────────── */
int pe_load(const void *data, uint32_t size, const char *name,
            pe_loaded_image_t *img)
{
    if (!data || !img) return PE_ERR_NULL;
    memset(img, 0, sizeof(*img));

    /* Name kopieren */
    if (name) strncpy(img->name, name, sizeof(img->name) - 1);

    const uint8_t *raw = (const uint8_t *)data;

    /* ── Schritt 1: MZ-Header prüfen ──────────────────────────────── */
    if (size < sizeof(pe_dos_header_t)) return PE_ERR_SMALL;

    const pe_dos_header_t *dos = (const pe_dos_header_t *)raw;
    if (dos->e_magic != PE_MZ_SIGNATURE) {
        KERR("PE: '%s' hat keinen MZ-Header (0x%04X)\n", name, dos->e_magic);
        return PE_ERR_MZ;
    }

    /* ── Schritt 2: PE-Signatur prüfen ────────────────────────────── */
    uint32_t pe_off = dos->e_lfanew;
    if (pe_off + 4 > size) return PE_ERR_SMALL;

    uint32_t pe_sig = *(uint32_t *)(raw + pe_off);
    if (pe_sig != PE_PE_SIGNATURE) {
        KERR("PE: '%s' hat keine PE-Signatur (0x%08X)\n", name, pe_sig);
        return PE_ERR_PE;
    }

    /* ── Schritt 3: COFF-Header ───────────────────────────────────── */
    uint32_t coff_off = pe_off + 4;
    if (coff_off + sizeof(pe_file_header_t) > size) return PE_ERR_SMALL;

    const pe_file_header_t *coff = (const pe_file_header_t *)(raw + coff_off);
    if (coff->Machine != PE_MACHINE_I386) {
        KERR("PE: '%s' falsche Architektur (0x%04X, erwartet i386)\n",
             name, coff->Machine);
        return PE_ERR_ARCH;
    }

    /* ── Schritt 4: Optional Header ───────────────────────────────── */
    uint32_t opt_off = coff_off + sizeof(pe_file_header_t);
    if (opt_off + sizeof(pe_optional_header32_t) > size) return PE_ERR_SMALL;

    const pe_optional_header32_t *opt =
        (const pe_optional_header32_t *)(raw + opt_off);
    if (opt->Magic != PE_OPT_MAGIC_PE32) {
        KERR("PE: '%s' kein PE32 (Magic=0x%04X)\n", name, opt->Magic);
        return PE_ERR_OPT;
    }

    uint32_t image_base  = opt->ImageBase;
    uint32_t image_size  = opt->SizeOfImage;
    uint32_t entry_rva   = opt->AddressOfEntryPoint;

    KINFO("PE: '%s' ImageBase=0x%08X Size=%u EP=0x%08X Sections=%u\n",
          name, image_base, image_size, entry_rva, coff->NumberOfSections);

    /* ── Schritt 5: Speicher allozieren ────────────────────────────── */
    uint8_t *base = (uint8_t *)kzalloc(image_size);
    if (!base) {
        KERR("PE: '%s' kann %u Bytes nicht allozieren\n", name, image_size);
        return PE_ERR_ALLOC;
    }

    /* Headers kopieren */
    uint32_t hdr_size = opt->SizeOfHeaders;
    if (hdr_size > size) hdr_size = size;
    memcpy(base, raw, hdr_size);

    /* ── Schritt 6: Sections laden ────────────────────────────────── */
    uint32_t sec_off = opt_off + coff->SizeOfOptionalHeader;
    const pe_section_header_t *sections =
        (const pe_section_header_t *)(raw + sec_off);

    for (uint16_t i = 0; i < coff->NumberOfSections; i++) {
        const pe_section_header_t *sec = &sections[i];

        /* Section-Name für Logging */
        char sname[PE_SECTION_NAME_LEN + 1];
        memcpy(sname, sec->Name, PE_SECTION_NAME_LEN);
        sname[PE_SECTION_NAME_LEN] = '\0';

        uint32_t vaddr = sec->VirtualAddress;
        uint32_t vsize = sec->VirtualSize;
        uint32_t rdata = sec->SizeOfRawData;
        uint32_t foff  = sec->PointerToRawData;

        /* Bounds-Prüfung */
        if (vaddr + vsize > image_size) {
            KERR("PE: Section '%s' überläuft Image (RVA=0x%X, VSize=%u)\n",
                 sname, vaddr, vsize);
            kfree(base);
            return PE_ERR_SECTION;
        }

        /* Rohdaten kopieren */
        if (rdata > 0 && foff + rdata <= size) {
            uint32_t copy = (rdata < vsize) ? rdata : vsize;
            memcpy(base + vaddr, raw + foff, copy);
        }
        /* Rest (BSS) ist schon 0 durch kzalloc */

        KINFO("PE:   [%s] RVA=0x%08X VSize=%u RawSize=%u Flags=0x%08X\n",
              sname, vaddr, vsize, rdata, sec->Characteristics);
    }

    /* ── Schritt 7: Base-Relocations ──────────────────────────────── */
    int32_t delta = (int32_t)((uint32_t)base - image_base);
    if (delta != 0 && opt->NumberOfRvaAndSizes > PE_DIR_BASERELOC) {
        const pe_data_directory_t *dirs =
            (const pe_data_directory_t *)(raw + opt_off +
                sizeof(pe_optional_header32_t));
        uint32_t reloc_rva  = dirs[PE_DIR_BASERELOC].VirtualAddress;
        uint32_t reloc_size = dirs[PE_DIR_BASERELOC].Size;

        if (reloc_rva && reloc_size) {
            KINFO("PE: Relocations anwenden (Delta=%d, %u Bytes)\n",
                  delta, reloc_size);
            pe_apply_relocations(base, reloc_rva, reloc_size, delta);
        }
    }

    /* ── Ergebnis setzen ──────────────────────────────────────────── */
    img->base        = (uint32_t)base;
    img->image_size  = image_size;
    img->entry_point = (uint32_t)base + entry_rva;
    img->num_sections = coff->NumberOfSections;
    img->subsystem   = opt->Subsystem;
    img->loaded      = true;

    KINFO("PE: '%s' geladen @ 0x%08X, Entry @ 0x%08X\n",
          name, img->base, img->entry_point);
    return PE_OK;
}

/* ── pe_exec ─────────────────────────────────────────────────────── */
int pe_exec(pe_loaded_image_t *img)
{
    if (!img || !img->loaded) {
        KERR("PE: Kann nicht-geladenes Image ausführen\n");
        return -1;
    }

    KINFO("PE: Starte '%s' @ 0x%08X\n", img->name, img->entry_point);

    /* Entry-Point als Funktion aufrufen (cdecl, keine Parameter) */
    typedef int (*pe_entry_t)(void);
    pe_entry_t entry = (pe_entry_t)img->entry_point;
    int ret = entry();

    KINFO("PE: '%s' beendet mit Code %d\n", img->name, ret);
    return ret;
}

/* ── pe_unload ───────────────────────────────────────────────────── */
void pe_unload(pe_loaded_image_t *img)
{
    if (!img) return;
    if (img->loaded && img->base) {
        KINFO("PE: '%s' entladen (%u Bytes freigegeben)\n",
              img->name, img->image_size);
        kfree((void *)img->base);
    }
    memset(img, 0, sizeof(*img));
}

/* ── pe_init ─────────────────────────────────────────────────────── */
void pe_init(void)
{
    KINFO("PE: PE32-Loader initialisiert (i386)\n");
}
