.globl gdt_init

GDT:
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
    .word 0x0068 # limit (104)
    .word 0x0000 # base (0-15)
    .byte 0x00 # base (16-23)
    .byte 0x89 # P=1, DPL=00, Type=1001 (32-bit TSS available)
    .byte 0x00 # G=0, AVL=0, limit (16-19)=0000
    .byte 0x00 # base (24-31)

END_GDT:

GDT_PTR:
    .word END_GDT - GDT - 1
    .long GDT

tss_base:
    .long 0x00000000 # last TSS
    .long 0x00000000 # ESP0
    .long DATA_SEG   # SS0
    .long 0x00000000 # ESP1
    .long 0x00000000 # SS1
    .long 0x00000000 # ESP2
    .long 0x00000000 # SS2
    .long 0x00000000 # CR3
    .long 0x00000000 # EIP
    .long 0x00000000 # EFLAGS
    .long 0x00000000 # EAX
    .long 0x00000000 # ECX
    .long 0x00000000 # EDX
    .long 0x00000000 # EBX
    .long 0x00000000 # ESP
    .long 0x00000000 # EBP
    .long 0x00000000 # ESI
    .long 0x00000000 # EDI
    .long 0x00000000 # ES
    .long 0x00000000 # CS
    .long 0x00000000 # SS
    .long 0x00000000 # DS
    .long 0x00000000 # FS
    .long 0x00000000 # GS
    .long 0x00000000 # LDTR
    .word 0x0000 # debug flag
    .word 0x0000 # I/O Permission Bitmap offset

.set CODE_SEG, 0x08 # Ring 0
.set DATA_SEG, 0x10 # Ring 0
.set USER_CODE_SEG, 0x18 | 3 # Ring 3
.set USER_DATA_SEG, 0x20 | 3 # Ring 3
.set TSS_SEG, 0x28

gdt_init:
    lgdt GDT_PTR

    lea tss_base, %eax
    movw %ax, tss_base+2

    shrl $16, %eax
    movb %al, tss_base+4
    movb %ah, tss_base+7

    movl $0xC009F000, tss_base+4

    mov $TSS_SEG, %ax
    ltr %ax

    ret
