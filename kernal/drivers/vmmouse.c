/**
 * @file vmmouse.c
 * @brief VMware Mouse (VMMouse) driver implementation.
 */
#include "../include/vmmouse.h"
#include "../include/cpu.h"
#include "../include/serial.h"
#include "../include/klib.h"
#include "../include/idt.h"

static bool vmmouse_present = false;

/* ── Backdoor-Protokoll ──────────────────────────────────────────── */
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi;
} vmmouse_regs_t;

static void vmmouse_cmd(vmmouse_regs_t *regs)
{
    regs->eax = VMMOUSE_MAGIC;
    __asm__ volatile (
        "outl %%eax, (%%dx)"
        : "=a"(regs->eax), "=b"(regs->ebx), "=c"(regs->ecx), "=d"(regs->edx),
          "=S"(regs->esi), "=D"(regs->edi)
        : "a"(regs->eax), "b"(regs->ebx), "c"(regs->ecx), "d"(regs->edx),
          "S"(regs->esi), "D"(regs->edi)
        : "memory"
    );
}

/* ── Öffentlich: vmmouse_init ────────────────────────────────────── */
bool vmmouse_init(void)
{
    vmmouse_regs_t regs;

    /* Schritt 1: VMMouse Version abfragen */
    regs.ebx = 0;
    regs.ecx = VMMOUSE_CMD_GETVERSION;
    regs.edx = VMMOUSE_PORT;
    vmmouse_cmd(&regs);

    if (regs.eax == (uint32_t)-1 || regs.ebx != VMMOUSE_READ_ID) {
        return false;
    }

    /* Schritt 2: VMMouse aktivieren */
    regs.ebx = 0x45414552; /* 'REAE' */
    regs.ecx = VMMOUSE_CMD_ABSPOINTER_COMMAND;
    regs.edx = VMMOUSE_PORT;
    vmmouse_cmd(&regs);
    
    /* Aktivieren (Request Absolute Mode) */
    regs.ebx = 0x53424152; /* 'RABS' */
    regs.ecx = VMMOUSE_CMD_ABSPOINTER_COMMAND;
    regs.edx = VMMOUSE_PORT;
    vmmouse_cmd(&regs);

    vmmouse_present = true;
    KINFO("VMMouse: Absolute Maus erkannt und aktiviert (QEMU/VMware)\n");
    return true;
}

/* ── Öffentlich: vmmouse_poll ────────────────────────────────────── */
bool vmmouse_poll(void)
{
    if (!vmmouse_present) return false;

    vmmouse_regs_t regs;
    
    /* Status abfragen: Anzahl der Pakete im Queue */
    regs.ebx = 0;
    regs.ecx = VMMOUSE_CMD_ABSPOINTER_STATUS;
    regs.edx = VMMOUSE_PORT;
    vmmouse_cmd(&regs);

    uint32_t count = regs.eax & 0xFFFF;
    if (count == 0 || count == (uint32_t)-1) return false;

    /* Ein Paket lesen (4 DWORDS: status, x, y, z) */
    regs.ebx = 4;
    regs.ecx = VMMOUSE_CMD_ABSPOINTER_DATA;
    regs.edx = VMMOUSE_PORT;
    vmmouse_cmd(&regs);

    /* EAX = status/buttons, EBX = X, ECX = Y, EDX = Z */
    uint32_t status = regs.eax;
    uint32_t x      = regs.ebx;
    uint32_t y      = regs.ecx;

    /* Buttons: VMMouse-Bits auf PinguinOS-Bits mappen
       VMMouse: 0x20 = Left, 0x10 = Right, 0x08 = Middle
    */
    uint8_t buttons = 0;
    if (status & 0x20) buttons |= 1; /* MOUSE_BTN_LEFT   */
    if (status & 0x10) buttons |= 2; /* MOUSE_BTN_RIGHT  */
    if (status & 0x08) buttons |= 4; /* MOUSE_BTN_MIDDLE */
    
    extern void mouse_set_abs_position(int32_t x, int32_t y, uint8_t buttons);
    mouse_set_abs_position((int32_t)x, (int32_t)y, buttons);

    return true; 
}
