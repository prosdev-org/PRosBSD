#include <cpu/gdt.h>
#include <drivers/keyboard.h>
#include <drivers/pit.h>
#include <drivers/vga_tty.h>
#include <generated/version.h>
#include <interrupts/idt.h>
#include <memory/map.h>
#include <memory/map/e820.h>
#include <memory/pfa.h>
#include <memory/virtual/layout.h>
#include <memory/virtual/paging.h>
#include <shell/shell.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>

static void log(const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    printf("[%f] ", (double) timer_seconds_since_boot());
    vprintf(format, args);
    putchar('\n');
}

_Noreturn void _main() {
    {
        log("\x1b[33mPFA test:\x1b[0m");
        uintptr_t ptrs[100];

        log("\x1b[33mAllocating...\x1b[0m");
        for (size_t i = 0; i < sizeof(ptrs) / sizeof(*ptrs); i++) {
            ptrs[i] = pf_alloc();
        }

        log("\x1b[33mFreeing...\x1b[0m");
        for (size_t i = 0; i < sizeof(ptrs) / sizeof(*ptrs); i++) {
            pf_free(ptrs[i]);
        }
    }

    log("Initializing keyboard...");
    keyboard_init();

    printf("\nWelcome to PRosBSD v.%s!\n\n", VERSION_STRING);
    printf("\033[34m * Source Code:   \033[0mhttps://github.com/prosdev-org/PRosBSD\n\n");
    shell_loop();
}

int main() {
    vga_tty_clear();

    log("Initializing GDT...");
    gdt_init();

    log("Initializing IDT...");
    idt_init();

    log("Initializing PIT...");
    pit_init();

    log("Detecting memory...");
    {
        size_t size;
        const e820_entry_t *e820_map = e820_get_map(&size);

        log("Memory map provided by BIOS:");
        for (size_t i = 0; i < size; i++) {
            log("%d) 0x%x-0x%x Type: 0x%x, ACPI3 attr: 0x%x", i, (uint32_t) e820_map[i].address,
                (uint32_t) (e820_map[i].address + e820_map[i].length - 1), e820_map[i].type,
                e820_map[i].acpi3_attributes);
        }
    }

    log("Initializing Memory map...");
    mem_map_init();

    log("Initializing PFA...");
    pfa_init();

    {
        size_t size;
        const memory_block_t *memory_map = get_memory_map(&size);

        log("Memory map:");
        for (size_t i = 0; i < size; i++) {
            const char *type;
            switch (memory_map[i].type) {
                case MEMORY_FREE:
                    type = "FREE";
                    break;
                case MEMORY_BUSY:
                    type = "BUSY";
                    break;
            }
            log("%d) 0x%x-0x%x %s", i, (uint32_t) memory_map[i].base,
                (uint32_t) (memory_map[i].base + memory_map[i].length - 1), type);
        }
    }

    log("Initializing Paging...");
    paging_init();

    mem_virt_layout_setup_stack();
    _main();
}
