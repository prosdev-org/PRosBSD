#ifndef PCI_DEFS_H
#define PCI_DEFS_H

// registers of PCI namespace
#define PCI_VENDOR_ID                 0x00
#define PCI_DEVICE_ID                 0x02
#define PCI_COMMAND                   0x04
#define PCI_STATUS                    0x06
#define PCI_REVISION_ID               0x08
#define PCI_CLASS_PROG                0x09
#define PCI_CLASS_DEVICE              0x0a
#define PCI_CACHE_LINE_SIZE           0x0c
#define PCI_LATENCY_TIMER             0x0d
#define PCI_HEADER_TYPE               0x0e
#define PCI_BIST                      0x0f
#define PCI_BASE_ADDRESS_0            0x10
#define PCI_BASE_ADDRESS_1            0x14
#define PCI_BASE_ADDRESS_2            0x18
#define PCI_BASE_ADDRESS_3            0x1c
#define PCI_BASE_ADDRESS_4            0x20
#define PCI_BASE_ADDRESS_5            0x24
#define PCI_CARDBUS_CIS               0x28
#define PCI_SUBSYSTEM_VENDOR_ID       0x2c
#define PCI_SUBSYSTEM_ID              0x2e
#define PCI_EXPANSION_ROM_BASE        0x30
#define PCI_CAPABILITIES              0x34
#define PCI_INTERRUPT_LINE            0x3c
#define PCI_INTERRUPT_PIN             0x3d
#define PCI_MIN_GNT                   0x3e
#define PCI_MAX_LAT                   0x3f

// types
#define PCI_HEADER_TYPE_NORMAL        0
#define PCI_HEADER_TYPE_BRIDGE        1
#define PCI_HEADER_TYPE_CARDBUS       2

// classes
#define PCI_CLASS_BRIDGE              0x06
#define PCI_SUBCLASS_BRIDGE_PCI       0x04

// capabilities
#define PCI_CAP_ID_MSI                0x05
#define PCI_CAP_ID_MSIX               0x11
#define PCI_CAP_ID_HT                 0x08

#endif