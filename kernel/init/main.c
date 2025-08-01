#include <drivers/idt.h>
#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <generated/version.h>
#include <memory/map/e820.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    cleark();

    printf("Initializing idt...\n\r");
    idt_init();

    printf("Detecting memory...\n");

    size_t size;
    e820_entry_t *mem_map = e820_get_map(&size);

    uint64_t available_mem = 0;

    printf("Memory map provided by BIOS\n");
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

    printf("Available memory: %d KiB\n", available_mem / 1024);

    printf("Initializing keyboard...\n\r");
    keyboard_init();

    printf("Welcome to PRos32 v.%s!\n\r", VERSION_STRING);

    for (;;) {
        putck(getchark());
    }

    return 0;
}
