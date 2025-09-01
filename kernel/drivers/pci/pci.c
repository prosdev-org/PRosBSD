#include <drivers/pci.h>
#include <drivers/tty.h>
#include <core/panic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

pci_bus_t g_root_bus;
pci_dev_t* g_device_list = NULL;

pci_dev_t* create_device(pci_bus_t* parent, uint8_t bus, uint8_t device, uint8_t func) {
    pci_dev_t* dev = calloc(1, sizeof(pci_dev_t));
    if (!dev) {
        panic("pci: out of memory for device");
    }
    dev->parent = parent;
    dev->domain = parent->domain;
    dev->bus = bus;
    dev->device = device;
    dev->function = func;

    // add to list
    dev->next = g_device_list;
    g_device_list = dev;

    return dev;
}

void read_bar_info(pci_dev_t* dev, int index, uint8_t offset) {
    uint32_t original_value = pci_read_config(dev, offset, 4);
    pci_write_config(dev, offset, 4, 0xFFFFFFFF);
    uint32_t size_mask = pci_read_config(dev, offset, 4);
    pci_write_config(dev, offset, 4, original_value);

    dev->info.bar[index].base = original_value;
    dev->info.bar[index].size = ~(size_mask & ~0xF) + 1;
    dev->info.bar[index].flags = original_value & 0xF;
}

void read_header_info(pci_dev_t* dev) {
    dev->info.vendor_id = pci_read_config(dev, PCI_VENDOR_ID, 2);
    dev->info.device_id = pci_read_config(dev, PCI_DEVICE_ID, 2);
    dev->info.revision = pci_read_config(dev, PCI_REVISION_ID, 1);
    dev->info.class_api = pci_read_config(dev, PCI_CLASS_PROG, 1);
    dev->info.class_sub = pci_read_config(dev, PCI_CLASS_DEVICE, 1);
    dev->info.class_base = pci_read_config(dev, PCI_CLASS_DEVICE + 1, 1);
    dev->info.header_type = pci_read_config(dev, PCI_HEADER_TYPE, 1);
    dev->info.interrupt_line = pci_read_config(dev, PCI_INTERRUPT_LINE, 1);
    dev->info.interrupt_pin = pci_read_config(dev, PCI_INTERRUPT_PIN, 1);

    if (dev->info.header_type == PCI_HEADER_TYPE_NORMAL) {
        for (int i = 0; i < 6; i++) {
            read_bar_info(dev, i, PCI_BASE_ADDRESS_0 + (i * 4));
        }
    } else if (dev->info.header_type == PCI_HEADER_TYPE_BRIDGE) {
        for (int i = 0; i < 2; i++) {
            read_bar_info(dev, i, PCI_BASE_ADDRESS_0 + (i * 4));
        }
    }
}

bool pci_find_capability(pci_dev_t* dev, uint8_t cap_id, uint8_t* offset) {
    uint16_t status = pci_read_config(dev, PCI_STATUS, 2);
    if (!(status & (1 << 4))) {
        return false; // no capabilities
    }

    uint8_t cap_offset = pci_read_config(dev, PCI_CAPABILITIES, 1);
    while (cap_offset != 0) {
        uint8_t current_cap_id = pci_read_config(dev, cap_offset, 1);
        if (current_cap_id == cap_id) {
            if (offset) {
                *offset = cap_offset;
            }
            return true;
        }
        cap_offset = pci_read_config(dev, cap_offset + 1, 1);
    }
    return false;
}

void pci_scan_function(pci_bus_t* parent_bus, uint8_t bus, uint8_t device, uint8_t func) {
    pci_dev_t dummy_dev = {.bus = bus, .device = device, .function = func};
    uint16_t vendor_id = pci_read_config(&dummy_dev, PCI_VENDOR_ID, 2);
    if (vendor_id == 0xFFFF) {
        return; // no device
    }

    pci_dev_t* dev = create_device(parent_bus, bus, device, func);
    read_header_info(dev);

    pci_read_msi_info(dev);
    pci_read_msix_info(dev);


    if (dev->info.class_base == PCI_CLASS_BRIDGE && dev->info.class_sub == PCI_SUBCLASS_BRIDGE_PCI) {
        uint8_t secondary_bus = pci_read_config(dev, 0x19, 1);
        if (secondary_bus > bus) {
             pci_bus_t* child_bus = calloc(1, sizeof(pci_bus_t));
             if (!child_bus) panic("pci: out of memory for bus");
             child_bus->parent = dev;
             child_bus->bus = secondary_bus;
             child_bus->domain = dev->domain;
             dev->child = child_bus;
             pci_enumerate_bus(child_bus);
        }
    }
}

void pci_scan_device(pci_bus_t* bus, uint8_t device) {
    pci_dev_t dummy_dev = {.bus = bus->bus, .device = device, .function = 0};
    if (pci_read_config(&dummy_dev, PCI_VENDOR_ID, 2) == 0xFFFF) {
        return;
    }

    pci_scan_function(bus, bus->bus, device, 0);

    uint8_t header_type = pci_read_config(&dummy_dev, PCI_HEADER_TYPE, 1);
    if ((header_type & 0x80) != 0) { // multifunctional device
        for (uint8_t func = 1; func < 8; func++) {
            pci_scan_function(bus, bus->bus, device, func);
        }
    }
}

void pci_enumerate_bus(pci_bus_t* bus) {
    for (uint8_t device = 0; device < 32; device++) {
        pci_scan_device(bus, device);
    }
}

void pci_init() {
    printf("Initializing PCI subsystem...\n");
    memset(&g_root_bus, 0, sizeof(pci_bus_t));
    g_root_bus.bus = 0;
    g_root_bus.domain = 0;
    
    pci_enumerate_bus(&g_root_bus);
    printf("PCI enumeration complete.\n");
}

void pci_print_info() {
    printf("\nPCI Devices List:\n");
    printf(" B/D/F   | Vend | Dev  | Base Class | Sub Class | Rev | IRQ\n");
    printf("---------+------+------+------------+-----------+-----+-----\n");

    for (pci_dev_t* dev = g_device_list; dev != NULL; dev = dev->next) {
        printf("  %d/%d/%d  |0x%x|0x%x| 0x%x        |    0x%x    | 0x%x | 0x%x\n",
               dev->bus, dev->device, dev->function,
               dev->info.vendor_id, dev->info.device_id,
               dev->info.class_base, dev->info.class_sub,
               dev->info.revision, dev->info.interrupt_line);

        if (dev->msi.msi_capable)
            printf("         MSI capable, %d messages\n", dev->msi.message_count);
        if (dev->msix.msix_capable)
            printf("         MSI-X capable, %d messages\n", dev->msix.message_count);
    }
}
