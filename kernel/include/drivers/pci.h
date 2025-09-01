#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <drivers/pci_defs.h>

typedef struct {
    bool     msi_capable;
    uint8_t  capability_offset;
    uint32_t message_count;
    uint32_t configured_count;
    uint32_t start_vector;
    uint16_t control_value;
    uint32_t data_value;
    uint64_t address_value;
} msi_info_t;

typedef struct {
    bool     msix_capable;
    uint8_t  capability_offset;
    uint32_t message_count;
    uint8_t  table_bar;
    uint32_t table_offset;
    void* table_address;
    uint32_t configured_count;
    uint32_t start_vector;
} msix_info_t;

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  revision;
    uint8_t  class_api;
    uint8_t  class_sub;
    uint8_t  class_base;
    uint8_t  header_type;
    uint8_t  interrupt_line;
    uint8_t  interrupt_pin;
    struct {
        uint64_t base;
        uint32_t size;
        uint8_t  flags;
    } bar[6];
} pci_device_info_t;

struct pci_bus;

typedef struct pci_dev {
    struct pci_dev* next;
    struct pci_bus* parent;
    struct pci_bus* child; // if bridge
    uint8_t             domain;
    uint8_t             bus;
    uint8_t             device;
    uint8_t             function;
    pci_device_info_t   info;
    msi_info_t          msi;
    msix_info_t         msix;
} pci_dev_t;

typedef struct pci_bus {
    pci_dev_t* parent;
    pci_dev_t* child;
    uint8_t     domain;
    uint8_t     bus;
} pci_bus_t;

extern pci_bus_t g_root_bus;
extern pci_dev_t* g_device_list;

void pci_init();
void pci_print_info();

void pci_read_msi_info(pci_dev_t* dev);
void pci_read_msix_info(pci_dev_t* dev);
void pci_enumerate_bus(pci_bus_t* bus);

uint32_t pci_read_config(pci_dev_t* dev, uint16_t offset, uint8_t size);
void pci_write_config(pci_dev_t* dev, uint16_t offset, uint8_t size, uint32_t value);

bool pci_find_capability(pci_dev_t* dev, uint8_t cap_id, uint8_t* offset);

#endif