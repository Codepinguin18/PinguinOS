# PinguinOS – 100-Feature Implementierungsplan

Dieses Dokument beschreibt den Status aller 100 geplanten Features,
aufgeteilt in implementierte und geplante Features mit Implementierungshinweisen.

---

## Phase 1 – Speichermanagement

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 1 | **SLAB-Allocator** | ✅ Implementiert | `mm/slab.c` |
| 2 | **Copy-on-Write (CoW)** | ✅ Basis in `proc_fork()` | `mm/process.c` |
| 3 | **Demand Paging** | 🔧 Stub | Siehe unten |
| 4 | **Swap-Support** | 📋 Geplant | Benötigt Block-IO |
| 5 | **Shared Memory** | 📋 Geplant | Benötigt CoW + VMA |
| 6 | **VMA-Tracking** | ✅ Implementiert | `mm/process.c` |
| 7 | **NX-Bit** | ✅ Basis (PDE_FLAGS) | `include/paging.h` |
| 8 | **Recursive Paging** | 📋 Geplant | `mm/paging.c` erweitern |
| 9 | **Page Cache** | 📋 Geplant | Benötigt IDE + VFS |
| 10 | **Huge Pages** | ✅ PSE 4MB aktiv | `mm/paging.c` |

### Demand Paging (Feature #3) – Implementierungsanleitung

```c
// In cpu/idt.c: Page-Fault-Handler registrieren
void page_fault_handler(cpu_regs_t *regs) {
    uint32_t fault_addr = read_cr2();
    uint32_t err = regs->err_code;

    // Prüfen ob es ein Demand-Paging-Fault ist
    process_t *proc = proc_current();
    vma_t *vma = vma_find(proc, fault_addr);
    if (!vma) kpanic("Segmentation Fault @ 0x%08x", fault_addr);

    // Neue physische Seite allozieren und mappen
    uint32_t phys = pmm_alloc_page();
    uint32_t virt = ALIGN_DOWN(fault_addr, PAGE_SIZE);
    uint32_t pflags = PDE_PRESENT | PDE_USER;
    if (vma->flags & VMA_WRITE) pflags |= PDE_WRITABLE;
    paging_map(virt, phys, pflags);
}
// Registrierung: exception_register(14, page_fault_handler);
```

### Shared Memory (Feature #5)

```c
// Zwei VMAs auf dieselbe physische Seite mappen:
uint32_t phys = pmm_alloc_page();
paging_map(virt_a, phys, PDE_PRESENT | PDE_WRITABLE | PDE_USER);
paging_map(virt_b, phys, PDE_PRESENT | PDE_WRITABLE | PDE_USER);
// Referenz-Zähler für die physische Seite einführen!
```

---

## Phase 2 – Prozess- & Thread-Management

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 11 | **Fork-Syscall** | ✅ `proc_fork()` | `mm/process.c` |
| 12 | **Signale (POSIX)** | ✅ Vollständig | `mm/process.c` |
| 13 | **Priority-Scheduler** | 🔧 Stub | `sched/scheduler.c` |
| 14 | **MLFQ** | 📋 Geplant | Erweitert Prio-Scheduler |
| 15 | **Wait Queues** | ✅ Implementiert | `sync/sync.c` |
| 16 | **Kernel-Threading-API** | ✅ `task_create()` | `sched/scheduler.c` |
| 17 | **I/O-Task-Priorisierung** | 📋 Geplant | |
| 18 | **Process Groups** | ✅ pgid/sid in PCB | `mm/process.c` |
| 19 | **Real-Time Scheduling** | 📋 Geplant | |
| 20 | **SMP** | 📋 Geplant | Benötigt Local APIC |

### Priority-Scheduler (Feature #13) – Erweiterung

```c
// In scheduler.h: task_t um Priorität erweitern:
typedef struct task {
    // ...bestehende Felder...
    uint8_t  priority;        // 0=niedrig ... 255=höchst
    uint8_t  base_priority;
    uint32_t quantum_ticks;   // Verbleibende Ticks
} task_t;

// In scheduler.c: sched_next() nach Priorität sortieren:
static task_t *sched_next(void) {
    task_t *best = NULL;
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (tasks[i].state != TASK_READY) continue;
        if (!best || tasks[i].priority > best->priority)
            best = &tasks[i];
    }
    return best;
}
```

---

## Phase 3 – Hardware-Treiber

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 31 | **PS/2 Tastatur** | ✅ DE/US-Layout | `drivers/keyboard.c` |
| 32 | **PS/2 Maus** | 📋 Geplant | `drivers/mouse.c` |
| 33 | **RTC (CMOS)** | ✅ Vollständig | `drivers/rtc.c` |
| 34 | **HPET** | ✅ Vollständig | `drivers/rtc.c` |
| 35 | **ACPI-Parser** | ✅ RSDP/RSDT/FADT | `drivers/acpi.c` |
| 36 | **Power Management** | ✅ Shutdown/Reboot | `drivers/acpi.c` |
| 37 | **PCIe (ECAM)** | 📋 Geplant | Erweitert `drivers/pci.c` |
| 38–40 | **USB Stack** | 📋 Sehr komplex | Mehrere Monate Aufwand |
| 41 | **IDE/PATA** | ✅ PIO-Mode | `drivers/ide.c` |
| 42 | **AHCI/SATA** | 📋 Geplant | Benötigt PCI BAR-Mapping |
| 43 | **NVMe** | 📋 Geplant | Benötigt PCIe |
| 44 | **E1000 NIC** | 🔧 Interface bereit | `net/net.c` |
| 45 | **RTL8139** | 📋 Geplant | |
| 46–47 | **AC97 / HDA** | 📋 Geplant | |
| 48 | **BGA Grafik** | 🔧 Stub | Siehe unten |
| 49 | **Floppy** | 📋 Geplant | |
| 50 | **UART IRQ** | ✅ Blockierend | `drivers/serial.c` |

### E1000-NIC-Treiber (Feature #44) – Schnellstart

```c
// drivers/e1000.c – Minimal-Implementierung
#define E1000_CTRL    0x0000
#define E1000_STATUS  0x0008
#define E1000_RCTL    0x0100   // Receive Control
#define E1000_TCTL    0x0400   // Transmit Control

void e1000_init(pci_device_t *dev) {
    // MMIO-BAR0 mappen
    uint32_t mmio = dev->bar[0] & ~0xF;
    paging_map(mmio, mmio, PDE_PRESENT | PDE_WRITABLE | PDE_NOCACHE);

    volatile uint32_t *reg = (volatile uint32_t *)mmio;

    // Reset
    reg[E1000_CTRL/4] |= (1 << 26);
    while (reg[E1000_CTRL/4] & (1 << 26));

    // MAC-Adresse aus EEPROM lesen, RX/TX-Deskriptor-Ringe einrichten
    // Interrupt-Handler registrieren (PCI IRQ-Line)
    irq_register(dev->int_line, e1000_irq_handler);
}
```

### BGA-Treiber (Feature #48)

```c
// drivers/bga.c – Bochs Graphic Adapter
#define BGA_INDEX  0x01CE
#define BGA_DATA   0x01CF
#define BGA_ID     0x00
#define BGA_XRES   0x01
#define BGA_YRES   0x02
#define BGA_BPP    0x03
#define BGA_ENABLE 0x04

static void bga_write(uint16_t idx, uint16_t val) {
    outw(BGA_INDEX, idx);
    outw(BGA_DATA, val);
}

void bga_set_mode(uint16_t w, uint16_t h, uint16_t bpp) {
    bga_write(BGA_ENABLE, 0);       // VBE-Modus deaktivieren
    bga_write(BGA_XRES, w);
    bga_write(BGA_YRES, h);
    bga_write(BGA_BPP, bpp);
    bga_write(BGA_ENABLE, 1);       // Linearer Framebuffer aktiv
    // Framebuffer liegt bei 0xE0000000 (QEMU Standard)
}
```

---

## Phase 4 – Dateisysteme

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 46 | **VFS Layer** | ✅ Vollständig | `fs/vfs.c` |
| 47 | **RAMFS** | ✅ Vollständig | `fs/ramfs.c` |
| 48 | **Initrd** | 📋 Geplant | Multiboot-Module |
| 49 | **FAT32** | 📋 Geplant | Benötigt IDE |
| 50 | **Ext2** | 📋 Geplant | |
| 51 | **ISO9660** | 📋 Geplant | |
| 52 | **DevFS** | 📋 Geplant | Erweitert VFS |
| 53 | **ProcFS** | 📋 Geplant | |
| 54 | **SysFS** | 📋 Geplant | |
| 55 | **MBR/GPT Parsing** | 📋 Geplant | Benötigt IDE |

### FAT32-Implementierung (Feature #49) – Schlüsselstrukturen

```c
typedef struct {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;    // 512
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;           // 2
    // ...
    uint32_t fat_size_32;
    uint32_t root_cluster;        // Cluster 2
    // ...
} __attribute__((packed)) fat32_bpb_t;

// Cluster lesen:
uint32_t fat32_cluster_to_lba(fat32_bpb_t *bpb, uint32_t cluster) {
    uint32_t data_start = bpb->reserved_sectors +
                          bpb->fat_count * bpb->fat_size_32;
    return data_start + (cluster - 2) * bpb->sectors_per_cluster;
}
```

---

## Phase 5 – Netzwerk-Stack

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 61 | **Ethernet** | ✅ Vollständig | `net/net.c` |
| 62 | **ARP** | ✅ Request + Reply | `net/net.c` |
| 63 | **IPv4** | ✅ Senden + Empfangen | `net/net.c` |
| 64 | **ICMP** | ✅ Ping/Pong | `net/net.c` |
| 65 | **UDP** | ✅ Vollständig | `net/net.c` |
| 66 | **TCP** | 📋 Geplant | Sehr komplex |
| 67 | **TCP Fenster** | 📋 Geplant | |
| 68 | **Socket API** | ✅ UDP-Sockets | `net/net.c` |
| 69 | **DHCP Client** | 📋 Geplant | Baut auf UDP auf |
| 70 | **DNS** | 📋 Geplant | UDP Port 53 |
| 71 | **Loopback** | 📋 Geplant | Virtuelle NIC |
| 72 | **NIC Management** | ✅ Interface-Tabelle | `net/net.c` |
| 73 | **Paketfilter** | 📋 Geplant | |
| 74 | **Port-Verwaltung** | ✅ In Socket-API | `net/net.c` |
| 75 | **Netzstatistiken** | ✅ Vollständig | `net/net.c` |

### TCP-Implementierung (Feature #66) – State Machine

```c
typedef enum {
    TCP_CLOSED, TCP_LISTEN, TCP_SYN_SENT, TCP_SYN_RECEIVED,
    TCP_ESTABLISHED, TCP_FIN_WAIT_1, TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT, TCP_CLOSING, TCP_LAST_ACK, TCP_TIME_WAIT
} tcp_state_t;

typedef struct tcp_connection {
    tcp_state_t state;
    uint32_t    seq_num;      // Eigene Sequenznummer
    uint32_t    ack_num;      // Erwartete Sequenznummer des Partners
    uint16_t    local_port;
    uint16_t    remote_port;
    ip4_addr_t  remote_ip;
    uint16_t    window_size;
    // Sende-/Empfangs-Puffer...
} tcp_conn_t;
```

### DHCP-Client (Feature #69)

```c
// DHCP Discover auf Port 67 broadcast senden:
void dhcp_discover(net_iface_t *iface) {
    // DHCP-Paket aufbauen (BOOTP-basiert)
    // UDP Port 68 → 67, Broadcast 255.255.255.255
    // Nach DHCP-Offer: ACK senden, IP-Adresse übernehmen
}
```

---

## Phase 6 – System & IPC

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 76 | **Syscall INT 0x80** | ✅ Vollständig | `ipc/syscall.c` |
| 77 | **ELF32 Loader** | ✅ Vollständig | `ipc/syscall.c` |
| 78 | **User-Mode** | ✅ Ring-3-Übergang | `ipc/syscall.c` |
| 79 | **Pipes** | 📋 Geplant | Ringpuffer + VFS |
| 80 | **Unix Sockets** | 📋 Geplant | |
| 81 | **Message Queues** | 📋 Geplant | |
| 82 | **Env-Variablen** | 📋 Geplant | |
| 83 | **Exit-Code** | ✅ In `proc_exit()` | `mm/process.c` |
| 84 | **Signal Masking** | ✅ `sig_mask` im PCB | `mm/process.c` |
| 85 | **Process Accounting** | 🔧 Ticks im PCB | `mm/process.c` |

### Pipes (Feature #79)

```c
typedef struct pipe {
    uint8_t  buf[4096];
    uint32_t read_pos, write_pos;
    uint32_t count;
    mutex_t  lock;
    condvar_t not_empty, not_full;
} pipe_t;

int pipe_create(int fds[2]) {
    pipe_t *p = kmalloc(sizeof(pipe_t));
    mutex_init(&p->lock);
    condvar_init(&p->not_empty);
    condvar_init(&p->not_full);
    // fds[0] = Leseende, fds[1] = Schreibende
    // Als VFS-Knoten in /proc/pipes/ eintragen
}
```

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
| 89 | **CPU-Topologie** | 📋 Geplant | CPUID erweiterung |
| 90 | **Kernel-Module** | 📋 Geplant | Komplexes Linker-Work |

### Local APIC Timer (Feature #86)

```c
#define APIC_BASE      0xFEE00000   // Standard-Adresse
#define APIC_REG_TIMER 0x320
#define APIC_TIMER_DIV 0x3E0
#define APIC_TIMER_ICR 0x380

void apic_timer_init(uint32_t hz) {
    volatile uint32_t *apic = (volatile uint32_t *)APIC_BASE;
    paging_map(APIC_BASE, APIC_BASE, PDE_PRESENT|PDE_WRITABLE|PDE_NOCACHE);

    apic[APIC_TIMER_DIV/4] = 0x3;   // Teiler: 16
    apic[APIC_REG_TIMER/4] = 0x20020; // Periodisch, Vektor 32
    // Kalibrieren via PIT-Referenzmessung
}
```

---

## Phase 8 – Debugging & Tools

| # | Feature | Status | Datei |
|---|---------|--------|-------|
| 91 | **Debug-Shell** | ✅ Vollständig | `debug/debug_shell.c` |
| 92 | **Backtrace** | 📋 Geplant | EBP-Chain verfolgen |
| 93 | **GDB-Stub** | 📋 Geplant | GDB Remote Protocol |
| 94 | **Memory-Leak-Detektor** | 🔧 Stub | kmalloc-Tracking |
| 95 | **Stack-Smashing-Schutz** | 📋 Geplant | GCC `__stack_chk_fail` |
| 96 | **Profiler** | 📋 Geplant | PIT-IRQ-Sampling |
| 97 | **Kprobes** | 📋 Geplant | INT3-Hooking |
| 98 | **Crash-Dumps** | 📋 Geplant | RAM → IDE schreiben |
| 99 | **Watchdog Timer** | 📋 Geplant | RTC-Alarm |
| 100 | **Entropy Pool (RNG)** | 📋 Geplant | Hardware-Events mischen |

### Backtrace (Feature #92)

```c
void print_backtrace(void) {
    uint32_t *ebp;
    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp));

    serial_puts("Stack-Backtrace:\n");
    for (int i = 0; i < 16 && ebp; i++) {
        uint32_t eip = ebp[1];   // Return-Address
        if (eip < 0x100000 || eip > 0xE0000000) break;
        serial_printf("  #%d  0x%08x\n", i, eip);
        ebp = (uint32_t *)ebp[0]; // Vorheriger EBP
    }
}
// Aufruf in kpanic() ergänzen!
```

### Memory-Leak-Detektor (Feature #94)

```c
// In heap.c: Allokations-Tracking aktivieren
#ifdef HEAP_LEAK_DETECT
typedef struct alloc_record {
    void    *ptr;
    size_t   size;
    const char *file;
    int      line;
} alloc_record_t;

static alloc_record_t records[1024];

#define kmalloc(s) kmalloc_tracked(s, __FILE__, __LINE__)
void *kmalloc_tracked(size_t size, const char *file, int line);
void heap_leak_report(void);  // Alle nicht-freigegebenen Allokationen ausgeben
#endif
```

### Entropy Pool / RNG (Feature #100)

```c
static uint32_t entropy_pool[32];
static uint32_t entropy_pos = 0;

// Events als Entropie-Quelle nutzen:
void entropy_add(uint32_t value) {
    entropy_pool[entropy_pos % 32] ^= value ^ sched_uptime_ms();
    entropy_pos++;
}

// In IRQ-Handlern aufrufen:
// entropy_add(regs->eip ^ regs->eax);

uint32_t rand_u32(void) {
    // Einfacher LFSR über den Pool:
    uint32_t result = 0;
    for (int i = 0; i < 32; i++)
        result ^= entropy_pool[i];
    entropy_pool[0] ^= result;
    return result;
}
```

---

## Nächste Prioritäten (Empfehlung)

1. **Demand Paging** (#3) – Voraussetzung für User-Space
2. **PS/2 Maus** (#32) – Einfach, gut für GUI-Grundlagen
3. **FAT32** (#49) – Lesen von Dateien von IDE-Disk
4. **E1000-NIC** (#44) – Netzwerk in QEMU
5. **Backtrace in kpanic()** (#92) – Sofort nützlich
6. **TCP-Stack** (#66) – Großes Projekt, benötigt viel Zeit
7. **ELF-Loader testen** (#77) – Erstes User-Space-Programm

---

## Build-Hinweise

```bash
# Alle implementierten Features bauen:
cd kernel && make all

# Mit Disk-Image (für IDE-Treiber-Tests):
make run-disk

# Debug-Session starten:
make debug
# In Terminal 2:
i686-elf-gdb build/kernel.elf \
    -ex "target remote :1234" \
    -ex "break debug_shell_run" \
    -ex "continue"
```

---

*PinguinOS – Klein, aber wächst! 🐧*
