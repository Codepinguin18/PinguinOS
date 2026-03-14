/**
 * @file vmmouse.h
 * @brief VMware Mouse (VMMouse) driver for PinguinOS.
 *
 * Provides absolute mouse coordinates in VMs (QEMU, VMware).
 */
#ifndef _VMMOUSE_H
#define _VMMOUSE_H

#include "types.h"
#include "mouse.h"

#define VMMOUSE_MAGIC        0x564D5868   /* 'VMXh' */
#define VMMOUSE_PORT         0x5658       /* 'VX'   */

#define VMMOUSE_CMD_GETVERSION    10
#define VMMOUSE_CMD_ABSPOINTER_DATA  39
#define VMMOUSE_CMD_ABSPOINTER_STATUS 40
#define VMMOUSE_CMD_ABSPOINTER_COMMAND 41

#define VMMOUSE_READ_ID      0x45414552

/** Detect and initialize VMMouse if available. */
bool vmmouse_init(void);

/** Poll VMMouse for events. */
bool vmmouse_poll(void);

#endif /* _VMMOUSE_H */
