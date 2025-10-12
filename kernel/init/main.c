#include <cpu/gdt.h>
#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <generated/version.h>
#include <interrupts/idt.h>
#include <memory/map.h>
#include <memory/map/e820.h>
#include <memory/pfa.h>
#include <memory/virtual/paging.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    cleark();

    printf("Initializing GDT...\n");
    gdt_init();

    printf("Initializing IDT...\n");
    idt_init();

    printf("Detecting memory...\n");
    {
        size_t size;
        const e820_entry_t *e820_map = e820_get_map(&size);

        printf("Memory map provided by BIOS:\n");
        for (size_t i = 0; i < size; i++) {
            printf("%d) ", i);
            printf("%p-", e820_map[i].address);
            printf("%p ", e820_map[i].address + e820_map[i].length - 1);
            printf("Type: %p, ", e820_map[i].type);
            printf("ACPI3 attr: %p", e820_map[i].acpi3_attributes);
            printf("\n");
        }
    }

    printf("Initializing Memory map...\n");
    mem_map_init();

    printf("Initializing PFA...\n");
    pfa_init();

    {
        size_t size;
        const memory_block_t *memory_map = get_memory_map(&size);

        printf("Memory map:\n");
        for (size_t i = 0; i < size; i++) {
            printf("[%d] ", i);
            printf("%p ", memory_map[i].base);
            printf("- %p", memory_map[i].base + memory_map[i].length - 1);
            switch (memory_map[i].type) {
                case MEMORY_FREE:
                    printf(" FREE");
                    break;
                case MEMORY_BUSY:
                    printf(" BUSY");
                    break;
            }
            printf("\n");
        }
    }

    printf("Initializing Paging...\n");
    paging_init();

    {
        printf("\x1b[33m");
        printf("PFA test:\n");
        uint32_t ptrs[100];

        printf("Allocating...\n");
        for (size_t i = 0; i < 100; i++) {
            ptrs[i] = pf_alloc();
        }

        printf("Freeing...\n");
        for (size_t i = 0; i < 100; i++) {
            pf_free(ptrs[i]);
        }

        printf("\x1b[0m");
    }

    printf("Initializing keyboard...\n");
    keyboard_init();

    printf("\nWelcome to PRosBSD v.%s!\n\n", VERSION_STRING);
    printf("\033[34m * Source Code:   \033[0mhttps://github.com/pros-dev-fans/PRosBSD\n\n");

    printf("\033[1;32m~$\033[0m ");

    for (;;) {
        putck(getchark());
    }
}
