#include <arch/i386/cpu/gdt.hxx>

.globl Arch_I386_Cpu_Gdt_init

.section .data
.align 8

gdt:
    .quad 0

    # ring 0, code (base=0, limit=0xFFFFF)
    .word 0xFFFF       # limit (0-15)
    .word 0x0000       # base (0-15)
    .byte 0x00         # base (16-23)
    .byte 0x9A         # P=1, DPL=00, S=1, Type=1010 (Exec/Read)
    .byte 0xCF         # G=1, D/B=1, AVL=0, limit (16-19)=1111
    .byte 0x00         # base (24-31)

    # ring 0, data (base=0, limit=0xFFFFF)
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0x92         # P=1, DPL=00, S=1, Type=0010 (Read/Write)
    .byte 0xCF
    .byte 0x00

    # ring 3, code
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0xFA         # P=1, DPL=11, S=1, Type=1010
    .byte 0xCF
    .byte 0x00

    # ring 3, data
    .word 0xFFFF
    .word 0x0000
    .byte 0x00
    .byte 0xF2         # P=1, DPL=11, S=1, Type=0010
    .byte 0xCF
    .byte 0x00

tss_entry:
    .word 0x0067 # limit (104)
    .word 0x0000 # base (0-15)
    .byte 0x00 # base (16-23)
    .byte 0x89 # P=1, DPL=00, Type=1001 (32-bit TSS available)
    .byte 0x00 # G=0, AVL=0, limit (16-19)=0000
    .byte 0x00 # base (24-31)

gdt_end:

.align 4
gdt_ref:
    .word gdt_end - gdt - 1
    .long gdt

.align 4
tss_base:
    .long 0, 0, ARCH_I386_GDT_KERN_DATA_SEG
    .fill 22, 4, 0
    .word 0
    .word 0x68 # IO Bitmap offset = size(TSS)

.section .text

Arch_I386_Cpu_Gdt_init:
    lgdt gdt_ref
    ljmp $ARCH_I386_GDT_KERN_CODE_SEG, $.reload_cs
.reload_cs:
    mov $ARCH_I386_GDT_KERN_DATA_SEG, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    lea tss_base, %eax
    movw %ax, (tss_entry+2) # Base 0-15
    shrl $16, %eax
    movb %al, (tss_entry+4) # Base 16-23
    movb %ah, (tss_entry+7) # Base 24-31

    movl $0xC009F000, (tss_base+4)

    mov $ARCH_I386_GDT_TSS_SEG, %ax
    ltr %ax

    ret
