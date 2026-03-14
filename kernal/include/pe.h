/**
 * @file pe.h
 * @brief PE32 (Portable Executable) Loader für PinguinOS.
 *
 * Unterstützt grundlegendes Laden von PE32-Dateien (i386):
 *   - MZ/PE Header-Parsing
 *   - Section-Loading (.text, .data, .rdata, .bss)
 *   - Base-Relocations
 *   - Entry-Point-Erkennung
 *
 * NICHT unterstützt: DLL-Imports (kernel32.dll etc.),
 * TLS, SEH, 64-Bit PE32+.
 */
#ifndef _PE_H
#define _PE_H

#include "types.h"

/* ── MZ (DOS) Header ─────────────────────────────────────────────── */
#define PE_MZ_SIGNATURE   0x5A4D   /* 'MZ' */
#define PE_PE_SIGNATURE   0x00004550  /* 'PE\0\0' */

typedef struct __attribute__((packed)) {
    uint16_t e_magic;       /* MZ-Signatur (0x5A4D)              */
    uint16_t e_cblp;        /* Bytes auf letzter Seite           */
    uint16_t e_cp;          /* Seiten in der Datei               */
    uint16_t e_crlc;        /* Relocations                       */
    uint16_t e_cparhdr;     /* Header-Größe in Paragraphen       */
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;      /* Offset zum PE-Header              */
} pe_dos_header_t;

/* ── COFF File Header ────────────────────────────────────────────── */
#define PE_MACHINE_I386   0x014C

#define PE_CHAR_EXECUTABLE_IMAGE    0x0002
#define PE_CHAR_32BIT_MACHINE       0x0100
#define PE_CHAR_DLL                 0x2000

typedef struct __attribute__((packed)) {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} pe_file_header_t;

/* ── Optional Header (PE32) ──────────────────────────────────────── */
#define PE_OPT_MAGIC_PE32   0x010B

#define PE_SUBSYS_NATIVE     1
#define PE_SUBSYS_GUI        2
#define PE_SUBSYS_CONSOLE    3

typedef struct __attribute__((packed)) {
    uint16_t Magic;                 /* 0x010B für PE32               */
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;   /* RVA des Entry Points          */
    uint32_t BaseOfCode;
    uint32_t BaseOfData;
    uint32_t ImageBase;             /* Bevorzugte Ladeadresse        */
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOSVersion;
    uint16_t MinorOSVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;           /* Gesamtgröße im Speicher       */
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
} pe_optional_header32_t;

/* ── Data Directory ──────────────────────────────────────────────── */
#define PE_DIR_EXPORT           0
#define PE_DIR_IMPORT           1
#define PE_DIR_RESOURCE         2
#define PE_DIR_BASERELOC        5
#define PE_DIR_TLS              9
#define PE_NUM_DATA_DIRECTORIES 16

typedef struct __attribute__((packed)) {
    uint32_t VirtualAddress;    /* RVA des Verzeichnisses             */
    uint32_t Size;              /* Größe in Bytes                     */
} pe_data_directory_t;

/* ── Section Header ──────────────────────────────────────────────── */
#define PE_SCN_MEM_EXECUTE  0x20000000
#define PE_SCN_MEM_READ     0x40000000
#define PE_SCN_MEM_WRITE    0x80000000
#define PE_SCN_CNT_CODE     0x00000020
#define PE_SCN_CNT_IDATA    0x00000040
#define PE_SCN_CNT_UDATA    0x00000080

#define PE_SECTION_NAME_LEN  8

typedef struct __attribute__((packed)) {
    char     Name[PE_SECTION_NAME_LEN];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;     /* RVA                               */
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;   /* Datei-Offset                      */
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;    /* PE_SCN_* Flags                    */
} pe_section_header_t;

/* ── Base Relocation ─────────────────────────────────────────────── */
#define PE_REL_BASED_ABSOLUTE   0
#define PE_REL_BASED_HIGHLOW    3

typedef struct __attribute__((packed)) {
    uint32_t VirtualAddress;    /* Seiten-RVA                         */
    uint32_t SizeOfBlock;       /* Blockgröße inkl. Header            */
    /* Danach: uint16_t TypeOffset[] */
} pe_base_reloc_block_t;

/* ── Geladenes Image ─────────────────────────────────────────────── */
typedef struct {
    uint32_t  base;             /* Tatsächliche Ladeadresse           */
    uint32_t  image_size;       /* Gesamtgröße im Speicher            */
    uint32_t  entry_point;      /* Absolute Adresse des Entry Points  */
    uint16_t  num_sections;     /* Anzahl der Sections                */
    uint16_t  subsystem;        /* PE_SUBSYS_*                        */
    char      name[32];         /* Name der EXE                       */
    bool      loaded;           /* Erfolgreich geladen?               */
} pe_loaded_image_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief PE32-Datei aus dem Speicher (RAM) laden.
 * @param data    Zeiger auf den gesamten PE-Dateiinhalt.
 * @param size    Größe der Datei in Bytes.
 * @param name    Name für Logging.
 * @param[out] img  Geladenes Image.
 * @return 0 bei Erfolg, negativer Fehlercode bei Fehler.
 */
int pe_load(const void *data, uint32_t size, const char *name,
            pe_loaded_image_t *img);

/**
 * @brief Geladenes PE-Image ausführen.
 * @param img  Zuvor geladenes Image.
 * @return Rückgabewert des Entry Points.
 */
int pe_exec(pe_loaded_image_t *img);

/**
 * @brief Geladenes PE-Image entladen (Speicher freigeben).
 * @param img  Zu entladendes Image.
 */
void pe_unload(pe_loaded_image_t *img);

/** PE-Subsystem initialisieren. */
void pe_init(void);

#endif /* _PE_H */
