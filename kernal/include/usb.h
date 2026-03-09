/**
 * @file usb.h
 * @brief USB 1.1/2.0-Stack für PinguinOS – Features #38–40.
 *
 * Unterstützt:
 *   #38 – UHCI-Host-Controller (USB 1.1, bis 12 Mbit/s)
 *   #39 – USB-Geräteerkennung und Enumeration
 *   #40 – USB-HID (Tastatur/Maus via Interrupt-Endpoint)
 *
 * Schichtenarchitektur:
 *   USB-HID-Treiber (Tastatur/Maus)
 *       ↕  USB-Geräteschicht (Enumeration, Deskriptoren)
 *       ↕  UHCI Host-Controller-Treiber
 *       ↕  PCI
 */
#ifndef _USB_H
#define _USB_H

#include "types.h"
#include "pci.h"

/* ── USB-Geräteklassen ───────────────────────────────────────────── */
#define USB_CLASS_HID        0x03   /* Human Interface Device         */
#define USB_CLASS_MASS_STOR  0x08   /* Mass Storage                   */
#define USB_CLASS_HUB        0x09   /* USB-Hub                        */

/* ── USB-Subklassen (HID) ────────────────────────────────────────── */
#define USB_HID_SUBCLASS_BOOT   0x01
#define USB_HID_PROTO_KEYBOARD  0x01
#define USB_HID_PROTO_MOUSE     0x02

/* ── USB-Übertragungstypen ───────────────────────────────────────── */
#define USB_XFER_CONTROL     0x00
#define USB_XFER_ISOCHRONOUS 0x01
#define USB_XFER_BULK        0x02
#define USB_XFER_INTERRUPT   0x03

/* ── Standardanforderungen (Setup-Paket) ─────────────────────────── */
#define USB_REQ_GET_DESCRIPTOR  0x06
#define USB_REQ_SET_ADDRESS     0x05
#define USB_REQ_SET_CONFIG      0x09
#define USB_REQ_SET_IDLE        0x0A

/* ── Deskriptor-Typen ────────────────────────────────────────────── */
#define USB_DESC_DEVICE         0x01
#define USB_DESC_CONFIG         0x02
#define USB_DESC_STRING         0x03
#define USB_DESC_INTERFACE      0x04
#define USB_DESC_ENDPOINT       0x05
#define USB_DESC_HID            0x21

/* ── Maximale Anzahl USB-Geräte ──────────────────────────────────── */
#define USB_MAX_DEVICES  16
#define USB_MAX_PORTS     8

/* ── UHCI-Register-Offsets (I/O-Port-basiert) ───────────────────── */
#define UHCI_CMD        0x00   /* Kommandoregister                   */
#define UHCI_STS        0x02   /* Statusregister                     */
#define UHCI_INTR       0x04   /* Interrupt-Aktivierung              */
#define UHCI_FRNUM      0x06   /* Frame-Nummer                       */
#define UHCI_FLBASEADD  0x08   /* Frame-List-Basisadresse            */
#define UHCI_SOFMOD     0x0C   /* Start-of-Frame-Modulator           */
#define UHCI_PORTSC(n)  (0x10 + (n)*2)  /* Port-Status/Control n    */

/* ── UHCI-Befehlsregister-Bits ───────────────────────────────────── */
#define UHCI_CMD_RS     (1u << 0)   /* Run/Stop                      */
#define UHCI_CMD_HCRESET (1u << 1)  /* Host-Controller-Reset         */
#define UHCI_CMD_GRESET (1u << 2)   /* Globaler Reset                */

/* ── UHCI-Port-Status-Bits ───────────────────────────────────────── */
#define UHCI_PORT_CCS   (1u << 0)   /* Current Connect Status        */
#define UHCI_PORT_CSC   (1u << 1)   /* Connect Status Change         */
#define UHCI_PORT_EN    (1u << 2)   /* Port aktiviert                */
#define UHCI_PORT_LSDA  (1u << 8)   /* Low-Speed-Gerät               */
#define UHCI_PORT_RST   (1u << 9)   /* Port-Reset                    */

/* ── USB-Gerätedeskriptor (vereinfacht) ──────────────────────────── */
typedef struct PACKED {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} usb_device_desc_t;

/* ── USB-Geräteeintrag ───────────────────────────────────────────── */
typedef struct usb_device {
    bool              used;
    uint8_t           address;    /* USB-Geräteadresse 1–127         */
    uint8_t           port;       /* Port am Host-Controller         */
    uint8_t           dev_class;
    uint8_t           dev_subclass;
    uint8_t           dev_proto;
    uint16_t          vendor_id;
    uint16_t          product_id;
    uint8_t           max_packet; /* Max. Paketgröße EP0             */
    bool              low_speed;  /* true = 1,5 Mbit/s, false = 12  */
} usb_device_t;

/* ── Öffentliche API ─────────────────────────────────────────────── */

/**
 * @brief USB-Stack initialisieren.
 *
 * Sucht alle UHCI-Host-Controller via PCI (Klasse 0x0C, Unterklasse 0x03,
 * Prog-IF 0x00), setzt sie zurück und startet die Port-Überwachung.
 * Muss nach pci_init() aufgerufen werden.
 */
void usb_init(void);

/** @return Zeiger auf ein USB-Gerät anhand des Indexes, oder NULL. */
const usb_device_t *usb_get_device(uint32_t index);

/** @return Anzahl der aktuell erkannten USB-Geräte. */
uint32_t usb_device_count(void);

/**
 * @brief USB-Kontrollübertragung (Setup + ggf. Daten).
 *
 * @param dev    Zielgerät.
 * @param bmRequestType  Richtung/Typ/Empfänger.
 * @param bRequest       Anforderungscode.
 * @param wValue         Wertfeld.
 * @param wIndex         Indexfeld.
 * @param buf            Datenpuffer (IN: Empfang, OUT: Senden).
 * @param wLength        Datenlänge.
 * @return 0 bei Erfolg, <0 bei Fehler.
 */
int usb_control_transfer(usb_device_t *dev,
                         uint8_t  bmRequestType, uint8_t  bRequest,
                         uint16_t wValue,        uint16_t wIndex,
                         void    *buf,           uint16_t wLength);

/** USB-Status auf dem seriellen Port ausgeben. */
void usb_dump(void);

#endif /* _USB_H */