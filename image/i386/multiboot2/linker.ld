ENTRY(_start)

SECTIONS {
    . = 0x00100000;

    __kernel_start = .;

    .multiboot2.data : {
        __multiboot2_data_start = .;
        *(.multiboot2.data)
        __multiboot2_data_end = .;
    }

    .multiboot2.text : {
        __multiboot2_text_start = .;
        *(.multiboot2.text)
        __multiboot2_text_end = .;
    }

    . += 0xC0000000;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        __text_start = .;
        *(.text*)
        __text_end = .;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        __rodata_start = .;
        *(.rodata*)
        __rodata_end = .;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        __data_start = .;
        *(.data*)
        __data_end = .;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    .bootstrap_stack ALIGN(4K) : AT(ADDR(.bootstrap_stack) - 0xC0000000) {
        __bootstrap_stack_start = .;
        . += 0x3FF0;
        __bootstrap_stack_top = .;
        __bootstrap_stack_end = .;
    }

    .heap ALIGN(4K) : AT(ADDR(.heap) - 0xC0000000) {
        __heap_start = .;
        . += 0x10000;
        __heap_end = .;
    }

    __kernel_end = .;
}
