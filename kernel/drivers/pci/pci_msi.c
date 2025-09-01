#include <drivers/pci.h>
#include <stdlib.h>
#include <string.h>

void pci_read_msi_info(pci_dev_t* dev) {
    memset(&dev->msi, 0, sizeof(msi_info_t));
    if (!pci_find_capability(dev, PCI_CAP_ID_MSI, &dev->msi.capability_offset)) {
        return;
    }

    dev->msi.msi_capable = true;
    dev->msi.control_value = pci_read_config(dev, dev->msi.capability_offset + 2, 2);

    int message_count_pow = (dev->msi.control_value >> 1) & 0x7;
    dev->msi.message_count = 1 << message_count_pow;
}

void pci_read_msix_info(pci_dev_t* dev) {
    memset(&dev->msix, 0, sizeof(msix_info_t));
    if (!pci_find_capability(dev, PCI_CAP_ID_MSIX, &dev->msix.capability_offset)) {
        return;
    }

    dev->msix.msix_capable = true;
    uint16_t control = pci_read_config(dev, dev->msix.capability_offset + 2, 2);
    dev->msix.message_count = (control & 0x7FF) + 1;

    uint32_t table = pci_read_config(dev, dev->msix.capability_offset + 4, 4);
    dev->msix.table_bar = table & 0x7;
    dev->msix.table_offset = table & ~0x7;
}