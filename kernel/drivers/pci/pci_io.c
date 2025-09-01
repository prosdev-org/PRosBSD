#include <drivers/pci.h>
#include <util/io.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC


uint32_t pci_get_address(uint8_t bus, uint8_t device, uint8_t function, uint16_t offset) {
    return 0x80000000 | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xfc);
}

uint32_t pci_read_config(pci_dev_t* dev, uint16_t offset, uint8_t size) {
    uint32_t address = pci_get_address(dev->bus, dev->device, dev->function, offset);
    outl(address, PCI_CONFIG_ADDRESS);

    uint32_t result = inl(PCI_CONFIG_DATA);

    switch (size) {
        case 1:
            return (result >> ((offset & 3) * 8)) & 0xff;
        case 2:
            return (result >> ((offset & 2) * 8)) & 0xffff;
        case 4:
            return result;
    }
    return 0xffffffff;
}

void pci_write_config(pci_dev_t* dev, uint16_t offset, uint8_t size, uint32_t value) {
    uint32_t address = pci_get_address(dev->bus, dev->device, dev->function, offset);
    outl(address, PCI_CONFIG_ADDRESS);

    if (size == 4) {
        outl(value, PCI_CONFIG_DATA);
    } else {
        uint32_t current_val = inl(PCI_CONFIG_DATA);
        uint32_t shift = (offset & (4 - size)) * 8;
        uint32_t mask = (1 << (size * 8)) - 1;

        current_val &= ~(mask << shift);
        current_val |= (value & mask) << shift;
        outl(current_val, PCI_CONFIG_DATA);
    }
}