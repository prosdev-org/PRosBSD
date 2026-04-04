#define MULTIBOOT2_HEADER_MAGIC     0xE85250D6
#define MULTIBOOT2_HEADER_ARCH_I386 0

#define MULTIBOOT2_HEADER_TAG_END 0

.section .multiboot2.data, "a"

multiboot2_header:
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT2_HEADER_ARCH_I386
    .long multiboot2_header_end - multiboot2_header
    .long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCH_I386 + (multiboot2_header_end - multiboot2_header))
multiboot2_header_end_tag_start:
    .short MULTIBOOT2_HEADER_TAG_END
    .short 0
    .long 8
multiboot2_header_end_tag_end:
multiboot2_header_end:

.section .multiboot2.text, "ax"

.extern Kernel_entry

.globl _start
_start:
    cli

    mov $0x9F000, %esp

    call Paging_setup

    mov $0xC009F000, %esp

    call Kernel_entry
halt:
    cli
    hlt
    jmp halt

Paging_setup:
    # Page directory
    mov $0x8000, %edi
    mov $4096, %ecx
    xor %eax, %eax
    cld
    rep stosl

    # ID mapping Page Entry
    mov $0x9000, %edi
    mov $4096, %ecx
    rep stosl

    # High addr (0xC000_0000) mapping Page Entry
    mov $0xA000, %edi
    mov $4096, %ecx
    rep stosl

    # flags: 3 - present, read/write
    movl $0x9003, 0x8000

    # flags, as shown above
    movl $0xA003, 0x8C00

    mov $0x9000, %edi
    mov $0x00000003, %eax # flags, as shown above
Page_ID_mapping_loop:
    stosl
    add $0x1000, %eax
    cmp $0xA000, %edi
    jne Page_ID_mapping_loop

    mov $0xA000, %edi
    mov $0x00000003, %eax # flags, as shown above
Page_High_mapping_loop:
    stosl
    add $0x1000, %eax
    cmp $0xB000, %edi
    jne Page_High_mapping_loop

    mov $0x8000, %eax
    mov %eax, %cr3

    mov %cr0, %eax
    or $0x80000000, %eax # set the PG bit
    mov %eax, %cr0

    ret
