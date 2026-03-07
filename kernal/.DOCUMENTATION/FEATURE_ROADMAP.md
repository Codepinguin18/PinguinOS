# PinguinOS – 100-Feature Implementierungsplan (v2.2)

Dieses Dokument beschreibt den aktuellen Status aller 100 geplanten Features.

**Legende:** ✅ Vollständig · 🔧 Stub/Teilweise · 📋 Geplant

---

## Phase 1 – Speichermanagement

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 1 | **SLAB-Allocator** | ✅ Vollständig | `mm/slab.c` |
| 2 | **Copy-on-Write (CoW)** | ✅ Basis in `proc_fork()` | `mm/process.c` |
| 3 | **Demand Paging** | ✅ Vollständig | `mm/paging.c` |
| 4 | **Swap-Support** | 📋 Geplant | Benötigt Block-IO |
| 5 | **Shared Memory** | 📋 Geplant | Benötigt CoW + VMA |
| 6 | **VMA-Tracking** | ✅ Vollständig | `mm/process.c` |
| 7 | **NX-Bit** | ✅ Basis (PDE_FLAGS) | `include/paging.h` |
| 8 | **Recursive Paging** | 📋 Geplant | `mm/paging.c` erweitern |
| 9 | **Page Cache** | 📋 Geplant | Benötigt IDE + VFS |
| 10 | **Huge Pages** | ✅ PSE 4MB aktiv | `mm/paging.c` |

### Demand Paging (#3) – Implementiert
Page-Fault-Handler (Exception 14) registriert in `paging_init()`.
Heap- und User-VMA-Faults werden automatisch aufgelöst.
Segmentation Faults (außerhalb VMAs) lösen `kpanic()` + Backtrace aus.

---

## Phase 2 – Prozess- & Thread-Management

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 11 | **Fork-Syscall** | ✅ `proc_fork()` | `mm/process.c` |
| 12 | **Signale (POSIX)** | ✅ Vollständig | `mm/process.c` |
| 13 | **Priority-Scheduler** | ✅ Vollständig | `sched/scheduler.c` |
| 14 | **MLFQ** | 📋 Geplant | Erweitert Prio-Scheduler |
| 15 | **Wait Queues** | ✅ Vollständig | `sync/sync.c` |
| 16 | **Kernel-Threading-API** | ✅ `task_create()` | `sched/scheduler.c` |
| 17 | **I/O-Task-Priorisierung** | 📋 Geplant | |
| 18 | **Process Groups** | ✅ pgid/sid in PCB | `mm/process.c` |
| 19 | **Real-Time Scheduling** | 📋 Geplant | |
| 20 | **SMP** | 📋 Geplant | Benötigt Local APIC |

### Priority-Scheduler (#13) – Implementiert
`task_create_prio(fn, name, priority)` – Prioritäten 0–255 (TASK_PRIO_* Konstanten).
Aging-Mechanismus verhindert Starvation (Boost nach AGING_THRESHOLD Ticks).
Quantum-basiertes Round-Robin für gleiche Prioritäten.

---

## Phase 3 – Hardware-Treiber

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 31 | **PS/2 Tastatur** | ✅ DE/US-Layout | `drivers/keyboard.c` |
| 32 | **PS/2 Maus** | ✅ Vollständig | `drivers/mouse.c` |
| 33 | **RTC (CMOS)** | ✅ Vollständig | `drivers/rtc.c` |
| 34 | **HPET** | ✅ Vollständig | `drivers/rtc.c` |
| 35 | **ACPI-Parser** | ✅ RSDP/RSDT/FADT | `drivers/acpi.c` |
| 36 | **Power Management** | ✅ Shutdown/Reboot | `drivers/acpi.c` |
| 37 | **PCIe (ECAM)** | 📋 Geplant | Erweitert `drivers/pci.c` |
| 38–40 | **USB Stack** | 📋 Sehr komplex | Mehrere Monate Aufwand |
| 41 | **IDE/PATA** | ✅ PIO-Mode | `drivers/ide.c` |
| 42 | **AHCI/SATA** | 📋 Geplant | Benötigt PCI BAR-Mapping |
| 43 | **NVMe** | 📋 Geplant | Benötigt PCIe |
| 44 | **E1000 NIC** | ✅ Vollständig | `drivers/e1000.c` |
| 45 | **RTL8139** | 📋 Geplant | |
| 46–47 | **AC97 / HDA** | 📋 Geplant | |
| 48 | **BGA Grafik** | ✅ Vollständig | `drivers/bga.c` |
| 49 | **Floppy** | 📋 Geplant | |
| 50 | **UART IRQ** | ✅ Blockierend | `drivers/serial.c` |

### PS/2 Maus (#32) – Implementiert
IRQ12, 3-Byte-Protokoll, Event-Queue (64 Einträge), absolute Position + Button-State.
`mouse_init()` → `mouse_poll()` für Events.

### E1000 NIC (#44) – Implementiert
PCI-Detect (0x8086:0x100E), MMIO BAR0, EEPROM-MAC, 8x RX/TX-Descriptor-Ringe,
IRQ-getriebener Empfang, `net_iface_t`-Registrierung.

### BGA Grafik (#48) – Implementiert
VBE-Ports 0x01CE/0x01CF, linearer Framebuffer an 0xE0000000.
`bga_set_mode(w, h, bpp)`, `bga_put_pixel()`, `bga_fill_rect()`, `bga_clear()`.

---

## Phase 4 – Dateisysteme

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 46 | **VFS Layer** | ✅ Vollständig | `fs/vfs.c` |
| 47 | **RAMFS** | ✅ Vollständig | `fs/ramfs.c` |
| 48 | **Initrd** | 📋 Geplant | Multiboot-Module |
| 49 | **FAT32 (read-only)** | ✅ Vollständig | `fs/fat32.c` |
| 50 | **Ext2** | 📋 Geplant | |
| 51 | **ISO9660** | 📋 Geplant | |
| 52 | **DevFS** | ✅ Vollständig | `fs/devfs.c` |
| 53 | **ProcFS** | ✅ Vollständig | `fs/procfs.c` |
| 54 | **SysFS** | 📋 Geplant | |
| 55 | **MBR/GPT Parsing** | ✅ Vollständig | `fs/partition.c` |

### FAT32 (#49) – Implementiert (read-only)
BPB-Parsing, Cluster-Chain-Walking, FAT32-Lookup über `fat32_finddir()`.
8.3-Kurzname-Unterstützung, Cluster-Lesen via IDE-Treiber.
Mount via `vfs_mount("/mnt/disk", "ide0p0", "fat32", 0)`.

### DevFS (#52) – Implementiert
Standardgeräte: `/dev/null`, `/dev/zero`, `/dev/random`, `/dev/tty`, `/dev/hd0`.
`devfs_register(name, type, read_fn, write_fn)` für neue Treiber.

### ProcFS (#53) – Implementiert
Virtuelle Dateien: `uptime`, `meminfo`, `cpuinfo`, `net/dev`, `tasks`.
Inhalte werden bei jedem `read()` on-the-fly generiert.

### MBR/GPT (#55) – Implementiert
Auto-Erkennung via Protective-MBR-Typ-Byte (0xEE).
GPT: Header-Parsing, bis zu 128 Einträge, UTF-16-Namen → ASCII.
MBR: 4 primäre Partitionen, Bootable-Flag.

---

## Phase 5 – Netzwerk-Stack

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 61 | **Ethernet** | ✅ Vollständig | `net/net.c` |
| 62 | **ARP** | ✅ Request + Reply | `net/net.c` |
| 63 | **IPv4** | ✅ Senden + Empfangen | `net/net.c` |
| 64 | **ICMP** | ✅ Ping/Pong | `net/net.c` |
| 65 | **UDP** | ✅ Vollständig | `net/net.c` |
| 66 | **TCP** | ✅ Vollständig | `net/tcp.c` |
| 67 | **TCP Fenster / Flow Control** | ✅ Vollständig | `net/tcp.c` |
| 68 | **Socket API** | ✅ UDP + TCP Sockets | `net/net.c` |
| 69 | **DHCP Client** | ✅ DORA-Exchange | `net/dhcp.c` |
| 70 | **DNS Client** | ✅ A-Record Resolver | `net/dns.c` |
| 71 | **Loopback** | ✅ 127.0.0.1/8 | `net/loopback.c` |
| 72 | **NIC Management** | ✅ Interface-Tabelle | `net/net.c` |
| 73 | **Paketfilter** | 📋 Geplant | |
| 74 | **Port-Verwaltung** | ✅ In Socket-API | `net/net.c` |
| 75 | **Netzstatistiken** | ✅ Vollständig | `net/net.c` |

### TCP (#66 + #67) – Implementiert
Vollständige 11-Zustands-State-Machine (RFC 793).
Aktiver Connect (SYN → SYN-ACK → ACK), graceful Close (FIN-Handshake).
Sliding Window, Retransmission-Timeout, Pseudo-Header-Prüfsumme.
API: `tcp_socket()`, `tcp_connect()`, `tcp_send()`, `tcp_recv()`, `tcp_close()`.

### DNS (#70) – Implementiert
UDP-A-Record-Resolver gegen konfigurierbaren DNS-Server (Standard: 8.8.8.8).
`dns_resolve("example.com", &ip, 5000)`.

### Loopback (#71) – Implementiert
`lo`-Interface mit 127.0.0.1/8. Sendet Pakete direkt zurück in `net_receive()`.

---

## Phase 6 – System & IPC

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 76 | **Syscall INT 0x80** | ✅ Vollständig | `ipc/syscall.c` |
| 77 | **ELF32 Loader** | ✅ Vollständig | `ipc/syscall.c` |
| 78 | **User-Mode** | ✅ Ring-3-Übergang | `ipc/syscall.c` |
| 79 | **Pipes** | ✅ Vollständig | `ipc/pipe.c` |
| 80 | **Unix Sockets** | 📋 Geplant | |
| 81 | **Message Queues** | 📋 Geplant | |
| 82 | **Env-Variablen** | ✅ Vollständig | `ipc/env.c` |
| 83 | **Exit-Code** | ✅ In `proc_exit()` | `mm/process.c` |
| 84 | **Signal Masking** | ✅ `sig_mask` im PCB | `mm/process.c` |
| 85 | **Process Accounting** | 🔧 Ticks im PCB | `mm/process.c` |

### Pipes (#79) – Implementiert
4 KB Ring-Buffer, Mutex + Condvar-geschützt, blockierendes Lesen/Schreiben.
`pipe_create(&read_node, &write_node)` liefert zwei VFS-Nodes.
Broken-Pipe-Erkennung beim Schreiben (ref_read == 0).

### Env-Variablen (#82) – Implementiert
`env_block_t` mit 64 Variablen. Globales Kernel-Env + per-Prozess-Kopie via `env_clone()`.
Standard-Variablen: `OS`, `VERSION`, `ARCH`, `PATH`, `HOME`, `TERM`.

---

## Phase 7 – Synchronisation

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 81 | **Spinlocks** | ✅ Vollständig | `include/sync.h` |
| 82 | **Semaphore** | ✅ Vollständig | `sync/sync.c` |
| 83 | **Mutex** | ✅ Mit Owner-Tracking | `sync/sync.c` |
| 84 | **Condition Variables** | ✅ Vollständig | `sync/sync.c` |
| 85 | **RW-Locks** | ✅ Vollständig | `sync/sync.c` |
| 86 | **Local APIC Timer** | 📋 Geplant | Ersetzt PIT |
| 87 | **I/O APIC** | 📋 Geplant | |
| 88 | **IPI** | 📋 Geplant | Benötigt SMP |
| 89 | **CPU-Topologie** | 📋 Geplant | CPUID-Erweiterung |
| 90 | **Kernel-Module** | 📋 Geplant | Komplexes Linker-Work |

---

## Phase 8 – Debugging & Tools

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 91 | **Debug-Shell** | ✅ Vollständig | `debug/debug_shell.c` |
| 92 | **Backtrace** | ✅ Vollständig | `cpu/cpu.c` |
| 93 | **GDB-Stub** | 📋 Geplant | GDB Remote Protocol |
| 94 | **Memory-Leak-Detektor** | ✅ Vollständig | `mm/heap.c` |
| 95 | **Stack-Smashing-Schutz** | ✅ Vollständig | `cpu/ssp.c` |
| 96 | **Profiler** | 📋 Geplant | PIT-IRQ-Sampling |
| 97 | **Kprobes** | 📋 Geplant | INT3-Hooking |
| 98 | **Crash-Dumps** | 📋 Geplant | RAM → IDE schreiben |
| 99 | **Watchdog Timer** | ✅ Vollständig | `drivers/watchdog.c` |
| 100 | **Entropy Pool (RNG)** | ✅ Vollständig | `entropy/entropy.c` |

### Backtrace (#92) – Implementiert
`print_backtrace()` per EBP-Chain-Walking, automatisch in `kpanic()` aufgerufen.
Zeigt bis zu 20 Return-Adressen mit Tiefenindex an.

### Memory-Leak-Detektor (#94) – Implementiert
1024-Einträge-Tabelle (ptr, size, file, line).
Aktivierung: `-DHEAP_LEAK_DETECT` → `kmalloc/kfree` werden zu Tracking-Varianten.
Bericht via `heap_leak_report()` (Debug-Shell-Befehl empfohlen).

### Stack-Smashing-Schutz (#95) – Implementiert
`ssp.c`: `__stack_chk_guard` (per Boot randomisiert via RNG) + `__stack_chk_fail()`.
Build-Flag: `-fstack-protector-strong` (entfernt `-fno-stack-protector` im Makefile).
`ssp_init()` in `cmain()` nach `entropy_init()` aufrufen.

### Watchdog (#99) – Implementiert
RTC-Periodic-Interrupt (IRQ8, ~1024 Hz) als unabhängige Zeitbasis.
`watchdog_init(timeout_seconds)` → `watchdog_enable()` → periodisch `watchdog_pet()`.
Bei Timeout: serielle Warnung + ACPI-Reboot.

### Entropy Pool (#100) – Implementiert
32-Wort-Pool mit Galois-LFSR und XOR-Fold-Ausgabe.
`entropy_add(value)` aus IRQ-Handlern; `rand_u32()`, `rand_range()`, `rand_bytes()`.

---

## Gesamtstatus

| Phase | Gesamt | ✅ Fertig | 🔧 Stub | 📋 Geplant |
|-------|--------|-----------|---------|------------|
| 1 – Speicher | 10 | 6 | 0 | 4 |
| 2 – Prozesse | 10 | 7 | 0 | 3 |
| 3 – Treiber | 20 | 11 | 0 | 9 |
| 4 – Dateisystem | 10 | 6 | 0 | 4 |
| 5 – Netzwerk | 15 | 13 | 0 | 2 |
| 6 – IPC | 10 | 7 | 1 | 2 |
| 7 – Sync | 10 | 5 | 0 | 5 |
| 8 – Debug | 10 | 8 | 0 | 2 |
| **Gesamt** | **100** | **63** | **1** | **36** |

---

## Nächste Prioritäten (Empfehlung)

1. **Ext2 read-only** (#50) – Wichtiger als ISO9660 für echte Disks
2. **GDB-Stub** (#93) – Remote-Debugging via QEMU -s
3. **Local APIC Timer** (#86) – Präzisere Zeitbasis als PIT
4. **MLFQ** (#14) – Besseres Scheduling für I/O-intensive Tasks
5. **Shared Memory** (#5) – IPC-Grundlage für User-Space
6. **Initrd** (#48) – Statisches Root-FS aus Multiboot-Modul
7. **SysFS** (#54) – Pendant zu ProcFS für Geräte/Parameter
8. **Message Queues** (#81) – IPC-Kompletierung
9. **Crash-Dumps** (#98) – Kernel-Crash auf Disk schreiben
10. **Profiler** (#96) – PIT-Sampling für Hotspot-Analyse

---

## Neue Dateien – Übersicht

| Neue Datei | Zielort | Feature |
|------------|---------|---------|
| `mouse.h` | `include/mouse.h` | #32 |
| `mouse.c` | `drivers/mouse.c` | #32 |
| `bga.h` | `include/bga.h` | #48 |
| `bga.c` | `drivers/bga.c` | #48 |
| `e1000.h` | `include/e1000.h` | #44 |
| `e1000.c` | `drivers/e1000.c` | #44 |
| `entropy.h` | `include/entropy.h` | #100 |
| `entropy.c` | `entropy/entropy.c` | #100 |
| `dhcp.c` | `net/dhcp.c` | #69 |
| `tcp.h` | `include/tcp.h` | #66/#67 |
| `tcp.c` | `net/tcp.c` | #66/#67 |
| `dns.h` | `include/dns.h` | #70 |
| `dns.c` | `net/dns.c` | #70 |
| `loopback.c` | `net/loopback.c` | #71 |
| `pipe.h` | `include/pipe.h` | #79 |
| `pipe.c` | `ipc/pipe.c` | #79 |
| `env.h` | `include/env.h` | #82 |
| `env.c` | `ipc/env.c` | #82 |
| `watchdog.h` | `include/watchdog.h` | #99 |
| `watchdog.c` | `drivers/watchdog.c` | #99 |
| `ssp.c` | `cpu/ssp.c` | #95 |
| `procfs.c` | `fs/procfs.c` | #53 |
| `partition.h` | `include/partition.h` | #55 |
| `partition.c` | `fs/partition.c` | #55 |
| `fat32.c` | `fs/fat32.c` | #49 |
| `devfs.c` | `fs/devfs.c` | #52 |

## Makefile-Ergänzung

```makefile
SRCS += drivers/mouse.c drivers/bga.c drivers/e1000.c drivers/watchdog.c
SRCS += net/tcp.c net/dns.c net/loopback.c net/dhcp.c
SRCS += ipc/pipe.c ipc/env.c
SRCS += fs/fat32.c fs/devfs.c fs/procfs.c fs/partition.c
SRCS += entropy/entropy.c cpu/ssp.c

# Stack-Smashing-Schutz aktivieren (entfernt -fno-stack-protector):
CFLAGS := $(filter-out -fno-stack-protector,$(CFLAGS))
CFLAGS += -fstack-protector-strong
```

---

*PinguinOS – Klein, aber wächst! 🐧 (63/100 Features implementiert)*