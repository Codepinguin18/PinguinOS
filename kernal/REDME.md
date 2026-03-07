# 🐧 PinguinOS

Ein von Grund auf neu geschriebener x86 32-Bit Kernel in C, mit minimalem Assembly-Bootstrap.

---

## Projektstruktur

```
PinguinOS/
├── kernel/
│   ├── boot.S              – Multiboot-Header + Stack-Setup + Context-Switch-ASM
│   ├── kernel.c            – C-Einstiegspunkt (cmain), Subsystem-Init
│   ├── klib.c              – Freestanding String/Memory-Utilities (memset, printf…)
│   ├── linker.ld           – Linker-Script (Ladeadresse 1 MB)
│   ├── Makefile            – Build-System (all / iso / run / debug)
│   │
│   ├── include/
│   │   ├── types.h         – uint8_t … uint64_t, bool, NULL, ALIGN_UP …
│   │   ├── multiboot.h     – Multiboot-1-Strukturen
│   │   ├── gdt.h           – GDT-API
│   │   ├── idt.h           – IDT / IRQ-API
│   │   ├── mm.h            – Physischer Speicher-API
│   │   ├── paging.h        – Paging-API
│   │   ├── heap.h          – kmalloc / kfree-API
│   │   ├── vga.h           – VGA Text + Framebuffer-API
│   │   ├── serial.h        – Serieller Debug-Log-API
│   │   ├── pci.h           – PCI-Enumeration-API
│   │   ├── scheduler.h     – Round-Robin-Scheduler-API
│   │   ├── cpu.h           – CPUID, I/O-Ports, kpanic, KASSERT
│   │   └── klib.h          – String/Format-Utilities-API
│   │
│   ├── cpu/
│   │   ├── gdt.c           – GDT + TSS Initialisierung
│   │   ├── idt.c           – IDT-Setup, PIC-Remapping, ISR-Dispatch
│   │   ├── isr.S           – 48 ISR-Stubs (0–47), gemeinsamer Handler
│   │   └── cpu.c           – CPUID-Info, kpanic
│   │
│   ├── mm/
│   │   ├── phys_alloc.c    – Bitmap-Physischer-Page-Allocator
│   │   ├── paging.c        – x86 Zwei-Stufen-Paging (4 KB + 4 MB PSE)
│   │   └── heap.c          – First-Fit Linked-List Heap
│   │
│   ├── drivers/
│   │   ├── vga.c           – VGA 80×25 Text + 32-bpp Framebuffer + 8×8 Font
│   │   ├── serial.c        – 16550 UART (COM1, 115200 8N1)
│   │   └── pci.c           – PCI Type-1 Enumeration
│   │
│   └── sched/
│       └── scheduler.c     – Preemptiver Round-Robin-Scheduler (PIT IRQ0)
│
└── iso/
    └── boot/
        └── grub/
            └── grub.cfg    – GRUB-2 Bootloader-Konfiguration
```

---

## Schritt-für-Schritt Build-Anleitung

### 1. Cross-Compiler installieren

Der Kernel muss mit einem **i686-elf Cross-Compiler** gebaut werden – d.h. einem GCC,
der ELF-Binärdateien für x86-32 erzeugt, ohne Host-libc-Abhängigkeiten.

#### Option A – OSDev-Binaries (empfohlen für schnellen Start)

Vorkompilierte Binaries für Ubuntu/Debian:
```bash
# Binutils + GCC für i686-elf aus dem OSDev-PPA
sudo add-apt-repository ppa:twestford/osdev
sudo apt update
sudo apt install i686-elf-gcc i686-elf-binutils
```

#### Option B – Selbst kompilieren (empfohlen für Produktionsnutzung)

Folge der [OSDev GCC Cross-Compiler-Anleitung](https://wiki.osdev.org/GCC_Cross-Compiler):

```bash
# Abhängigkeiten
sudo apt install build-essential bison flex libgmp3-dev \
                 libmpc-dev libmpfr-dev texinfo

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Binutils 2.41
mkdir -p $HOME/src && cd $HOME/src
wget https://ftp.gnu.org/gnu/binutils/binutils-2.41.tar.xz
tar -xf binutils-2.41.tar.xz
mkdir build-binutils && cd build-binutils
../binutils-2.41/configure --target=$TARGET --prefix=$PREFIX \
    --with-sysroot --disable-nls --disable-werror
make -j$(nproc) && make install

# GCC 13.2.0
cd $HOME/src
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.xz
tar -xf gcc-13.2.0.tar.xz
mkdir build-gcc && cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix=$PREFIX \
    --disable-nls --enable-languages=c --without-headers
make -j$(nproc) all-gcc all-target-libgcc
make install-gcc install-target-libgcc
```

#### Option C – Docker (keine lokale Installation nötig)

```dockerfile
# Dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    gcc-i686-linux-gnu binutils-i686-linux-gnu \
    grub-pc-bin xorriso qemu-system-x86 make
```

```bash
docker build -t pinguinos-build .
docker run -v $(pwd):/src -w /src/kernel pinguinos-build make all iso
```

### 2. Build-Abhängigkeiten prüfen

```bash
# Mindestens diese Tools werden benötigt:
i686-elf-gcc --version    # Cross-Compiler
i686-elf-ld  --version    # Cross-Linker
grub-mkrescue --version   # ISO-Erzeugung
xorriso --version         # Wird von grub-mkrescue genutzt
qemu-system-i386 --version # Emulator
```

### 3. Kernel kompilieren

```bash
cd kernel/
make all
# Output: build/kernel.elf
```

Zu sehen sollten Ausgaben wie:
```
  AS    boot.S
  AS    cpu/isr.S
  CC    kernel.c
  CC    klib.c
  ...
  LD    build/kernel.elf
```

**Debug-Build** (ohne Optimierung, mit Debug-Symbolen):
```bash
make all CFLAGS="-m32 -std=c11 -ffreestanding -fno-stack-protector \
                  -fno-pic -fno-builtin -nostdlib -Wall -O0 -g \
                  -I./include"
```

### 4. Bootfähige ISO erzeugen

```bash
make iso
# Ruft grub-mkrescue auf und erzeugt PinguinOS.iso
```

Das Makefile kopiert `build/kernel.elf` nach `../iso/boot/kernel.elf` und
ruft anschließend `grub-mkrescue` auf.

Falls `grub-mkrescue` fehlt:
```bash
# Ubuntu/Debian
sudo apt install grub-pc-bin grub-common xorriso

# Arch Linux
sudo pacman -S grub xorriso
```

### 5. ISO in QEMU starten

```bash
make run
# Äquivalent zu:
qemu-system-i386 -cdrom PinguinOS.iso -m 512M -serial stdio \
                 -boot d -no-reboot -no-shutdown
```

**Direkter Kernel-Start** (ohne ISO, schneller für Entwicklung):
```bash
make run-kernel
# Äquivalent zu:
qemu-system-i386 -kernel build/kernel.elf -m 512M -serial stdio \
                 -no-reboot -no-shutdown
```

### 6. Debugging mit QEMU + GDB

**Terminal 1 – QEMU mit GDB-Stub starten:**
```bash
make debug
# QEMU startet, wartet auf GDB-Verbindung auf Port 1234 (-S -s flags)
```

**Terminal 2 – GDB verbinden:**
```bash
i686-elf-gdb build/kernel.elf \
    -ex "target remote :1234" \
    -ex "break cmain"         \
    -ex "continue"            \
    -ex "layout src"
```

Nützliche GDB-Befehle:
```gdb
(gdb) info registers        # Alle CPU-Register anzeigen
(gdb) x/10i $eip            # 10 Instruktionen ab EIP disassemblieren
(gdb) x/20wx $esp           # Stack-Inhalt anzeigen
(gdb) break pmm_init        # Breakpoint setzen
(gdb) watch cur_row         # Watchpoint auf Variable
(gdb) print pci_device_count()
```

**Serial-Debugging mit QEMU:**

Die `-serial stdio` Option leitet COM1 direkt ans Terminal weiter.
Alle `KINFO()`, `KWARN()`, `KERR()` Meldungen erscheinen dort:

```bash
# Nur serielle Ausgabe (kein VGA-Fenster)
qemu-system-i386 -kernel build/kernel.elf -m 512M -serial stdio -nographic
```

---

## Kernel-Architektur

### Boot-Sequenz

```
GRUB
  └─> boot.S (_start)
        – Multiboot-Magic geprüft
        – Stack eingerichtet (16 KB in BSS)
        – cmain(magic, mbi) aufgerufen
              └─> kernel.c (cmain)
                    1. VGA + Serial init
                    2. GDT laden (gdt_flush via ASM)
                    3. IDT laden + PIC remappen
                    4. CPUID
                    5. PMM init (Bitmap aus Multiboot-Memmap)
                    6. Paging enable (PSE 4MB, Identity + Higher-Half)
                    7. Heap init (kmalloc/kfree)
                    8. PCI scan
                    9. Scheduler init (PIT 100Hz)
                   10. Demo-Tasks erstellen
                   11. STI + HLT-Schleife
```

### Speichermap

```
Physisch:
  0x00000000 – 0x000FFFFF  : Niedrige 1 MB (BIOS, VGA, GRUB)
  0x00100000 – _kernel_end : Kernel-Image
  _kernel_end – +32 KB     : PMM-Bitmap
  0xD0000000 – 0xE0000000  : Kernel-Heap (virtuell, lazy allocated)

Virtuell (nach paging_init):
  0x00000000 – 0x003FFFFF  : Identity-Map (4 MB PSE-Page)
  0xC0000000 – 0xC03FFFFF  : Kernel Higher-Half (4 MB PSE-Page)
  0xD0000000 – 0xDFFFFFFF  : Kernel-Heap
```

### Subsystem-APIs

#### Speicherverwaltung

```c
// Physischer Page-Allocator
uint32_t pmm_alloc_page(void);           // 1 Seite (4 KB) allozieren
void     pmm_free_page(uint32_t addr);   // Seite freigeben
uint32_t pmm_free_page_count(void);      // Freie Seiten

// Paging
void     paging_map(uint32_t virt, uint32_t phys, uint32_t flags);
void     paging_unmap(uint32_t virt);
uint32_t paging_get_phys(uint32_t virt);

// Heap (kmalloc-Familie)
void    *kmalloc(size_t size);
void    *kzalloc(size_t size);           // Nullinitialisiert
void    *krealloc(void *ptr, size_t n);
void     kfree(void *ptr);
```

#### IRQ-Verwaltung

```c
// Handler registrieren (irq = 0..15, Hardware-IRQ-Nummer)
void irq_register(uint8_t irq, irq_handler_t handler);
void irq_unregister(uint8_t irq);

// Handler-Signatur
typedef void (*irq_handler_t)(cpu_regs_t *regs);

// Beispiel: eigenen Keyboard-Handler installieren
void my_kb_handler(cpu_regs_t *regs) {
    uint8_t scancode = inb(0x60);
    // scancode verarbeiten…
}
irq_register(IRQ_KEYBOARD, my_kb_handler);
```

#### Scheduling

```c
// Neuen Kernel-Thread erstellen
task_t *task_create(task_fn_t fn, const char *name);
void    task_exit(int exit_code);    // NORETURN
void    task_yield(void);
void    task_sleep(uint32_t ms);
task_t *sched_current(void);
uint32_t sched_uptime_ms(void);
```

#### PCI

```c
void          pci_init(void);
pci_device_t *pci_find_class(uint8_t class_code, uint8_t subclass);
pci_device_t *pci_find_device(uint16_t vendor, uint16_t device_id);
uint32_t      pci_read32(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);
void          pci_write32(…, uint32_t val);
```

#### Kernel-Ausgabe

```c
// VGA Text-Modus
vga_printf("Hello %s! Value: %d\n", "Welt", 42);
vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);

// Serial Log (QEMU -serial stdio)
KINFO("Speicher: %u MB frei\n", free_mb);
KWARN("Warnung: %s\n", msg);
KERR("Fehler!\n");

// Panic (hält die CPU an)
kpanic("Fehler bei %s:%d\n", __FILE__, __LINE__);
KASSERT(ptr != NULL);   // Panic wenn false
```

---

## Häufige Probleme

| Symptom | Ursache | Lösung |
|---------|---------|--------|
| `i686-elf-gcc: command not found` | Cross-Compiler nicht installiert | Abschnitt 1 oben |
| `grub-mkrescue: command not found` | GRUB-Tools fehlen | `apt install grub-pc-bin xorriso` |
| Triple Fault direkt beim Boot | Stack/GDT falsch | `make debug` + GDB |
| Schwarzer Bildschirm | VGA-Adresse falsch | 0xB8000 prüfen |
| QEMU: `KVM not available` | Virtualisierung deaktiviert | `-accel tcg` hinzufügen |
| Linker: `undefined reference` | Objekt-Datei fehlt | Alle .c in Makefile prüfen |

---

## Erweiterungsideen

- **Keyboard-Treiber**: IRQ1-Handler + PS/2-Scancode-Tabelle
- **ATA/IDE-Treiber**: PIO-Mode für Festplattenzugriff (IRQ14/15)
- **FAT32-Dateisystem**: Auf ATA-Treiber aufbauend
- **User-Space / System Calls**: Ring-3-Tasks + INT 0x80
- **ACPI**: Shutdown, Power-Management
- **SMP**: Multi-Core via APIC
- **E1000-Netzwerktreiber**: PCI-Device 8086:100E (QEMU default NIC)

---

## Lizenz

PinguinOS ist freie Software für Lern- und Experimentierzwecke.