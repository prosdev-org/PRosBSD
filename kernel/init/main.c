#include <drivers/idt.h>
#include <drivers/keyboard.h>
#include <drivers/pata_pio.h>
#include <drivers/tty.h>
#include <fs/fat32.h>
#include <generated/version.h>
#include <memory/map/e820.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to get disk size in MiB
void get_disk_size(uint32_t *total_mb) {
    uint8_t boot_sector[512];
    if (!pata_pio_read_sector(0, boot_sector)) {
        *total_mb = 0;
        return;
    }

    fat32_boot_sector_t *bs = (fat32_boot_sector_t *) boot_sector;

    // Calculate total size
    uint32_t total_sectors = bs->total_sectors_32;
    if (total_sectors == 0) {
        total_sectors = bs->total_sectors_16;
    }

    *total_mb = (total_sectors * bs->bytes_per_sector) / (1024 * 1024);
}

int main(void) {
    cleark();

    // Initialize IDT
    printf("Initializing IDT...\n");
    idt_init();

    printf("Detecting memory...\n");

    size_t size;
    e820_entry_t *mem_map = e820_get_map(&size);
    uint64_t available_mem = 0;

    printf("Memory map provided by BIOS:\n");
    for (size_t i = 0; i < size; i++) {
        if (mem_map[i].type == 1) {
            available_mem += mem_map[i].length;
        }

        printf("[%d] = ", i);
        printf("Address: %p-", mem_map[i].address);
        printf("%p, ", mem_map[i].address + mem_map[i].length - 1);
        printf("Type: %p, ", mem_map[i].type);
        printf("ACPI3 attr: %p", mem_map[i].acpi3_attributes);
        printf("\n");
    }

    free(mem_map);

    printf("Available memory: %d MiB\n", available_mem / (1024 * 1024));

    // Initialize keyboard
    printf("Initializing keyboard...\n");
    keyboard_init();

    // Welcome message and system information
    printf("\nWelcome to PRos32 v.%s!\n\n", VERSION_STRING);

    printf("\033[34m * Documentation: \033[0mhttps://github.com/pros-dev-fans/PRos32\n");
    printf("\033[34m * Support:       \033[0mhttps://github.com/pros-dev-fans/PRos32\n");
    printf("\033[34m * Source Code:   \033[0mhttps://github.com/pros-dev-fans/PRos32\n\n");

    printf("\033[1;32m System information\033[0m\n\n");

    // Get disk size
    uint32_t total_mb;
    get_disk_size(&total_mb);
    printf(" * Disk size:   %d MiB\n", total_mb);

    // Memory usage
    printf(" * Memory size: %d MiB\n\n", (uint32_t) (available_mem / (1024 * 1024)));

    printf("\033[1;32m~$\033[0m ");

    for (;;) {
        putck(getchark());
    }

    return 0;
}
