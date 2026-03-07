/**
 * @file syscall.h
 * @brief Syscall-Gateway (INT 0x80) + ELF32-Loader (Features #76, #77, #78).
 */
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "idt.h"

/* ── Syscall-Nummern ─────────────────────────────────────────────── */
#define SYS_EXIT      1
#define SYS_FORK      2
#define SYS_READ      3
#define SYS_WRITE     4
#define SYS_OPEN      5
#define SYS_CLOSE     6
#define SYS_WAITPID   7
#define SYS_EXECVE    11
#define SYS_GETPID    20
#define SYS_KILL      37
#define SYS_SLEEP     162
#define SYS_YIELD     158
#define NR_SYSCALLS   256

typedef int32_t (*syscall_fn_t)(uint32_t a, uint32_t b, uint32_t c,
                                 uint32_t d, uint32_t e);

/* ── Öffentliche API ─────────────────────────────────────────────── */
void syscall_init(void);

/* ── ELF32-Loader ────────────────────────────────────────────────── */

/* ELF-Magic */
#define ELF_MAGIC0  0x7F
#define ELF_MAGIC1  'E'
#define ELF_MAGIC2  'L'
#define ELF_MAGIC3  'F'

/* ELF-Typen */
#define ET_EXEC  2    /* Ausführbare Datei */
#define EM_386   3    /* x86 32-bit        */

/* Segment-Typen */
#define PT_LOAD   1
#define PT_NULL   0

/* ELF-Header (32-bit) */
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;      /* Einsprungpunkt              */
    uint32_t e_phoff;      /* Program-Header-Offset       */
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;      /* Anzahl Program-Header       */
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} PACKED elf32_hdr_t;

/* Programm-Header (32-bit) */
typedef struct {
    uint32_t p_type;
    uint32_t p_offset;     /* Offset in der ELF-Datei     */
    uint32_t p_vaddr;      /* Virtuelle Zieladresse       */
    uint32_t p_paddr;
    uint32_t p_filesz;     /* Größe im File               */
    uint32_t p_memsz;      /* Größe im Speicher           */
    uint32_t p_flags;
    uint32_t p_align;
} PACKED elf32_phdr_t;

/**
 * @brief Lädt ein ELF32-Binary aus dem Speicher und bereitet die Ausführung vor.
 * @param elf_data   Zeiger auf den ELF-Datei-Inhalt im Speicher.
 * @param elf_size   Größe der ELF-Datei in Bytes.
 * @param entry_out  Einsprungpunkt wird hier gespeichert.
 * @return 0 bei Erfolg, negativ bei Fehler.
 */
int elf32_load(const uint8_t *elf_data, uint32_t elf_size,
               uint32_t *entry_out);

/**
 * @brief Wechselt in Ring-3 und startet Ausführung ab @p entry.
 * @param entry     Virtuelle Einsprungadresse.
 * @param user_esp  User-Stack-Top.
 *
 * Diese Funktion kehrt nie zurück.
 */
void NORETURN user_mode_enter(uint32_t entry, uint32_t user_esp);

#endif /* _SYSCALL_H */
