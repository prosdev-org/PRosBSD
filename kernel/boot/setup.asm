.code16
.globl _start

.set KERNEL_SIZE, 65536 / 4

_start:
    jmp start

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
    .word tss_base # base (0-15)
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
    .long 0x001FFFF0 # ESP0
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

font_data:
    .incbin "../../cfg/FONT.FNT"

DisableMotor: # FLoppy motor killer
    push %dx
    mov $0x3F2, %dx
    mov $0, %al
    out %al, %dx
    pop %dx
    ret

LoadFont:
    movb $0x11, %ah
    movb $0x00, %al
    movw $font_data, %bp
    movb $16, %bh
    movw $224, %cx
    movw $32, %dx
    movb $0, %bl
    int $0x10
    ret

DisableCursor:
    mov $0x01, %ah
    mov $0x3F, %ch
    int $0x10
    ret

.equ nmap_ent, 0x5000
DoE820:
    mov $nmap_ent + 0x0004, %di # otherwise we will stuck in int 0x15 after some entries are fetched
    xor %ebx, %ebx # %ebx must be 0
    xor %bp, %bp # entry count
    mov $0x534D4150, %edx # Place "SMAP" into edx
    mov $0xE820, %eax
    movl $1, %es:20(%di) # force a valid ACPI 3.x entry
    mov $24, %ecx # ask for 24 bytes
    int $0x15
    jc E820_failed # carry set means "Unsupported function"
    mov $0x534D4150, %eax # some BIOSes trash this register
    cmp %eax, %edx # on success, %eax must be set to "SMAP"
    jne E820_failed
    test %ebx, %ebx # if ebx = 0 it is worthless
    je E820_failed
    jmp E820_jmpin
E820_lp:
    mov $0xE820, %eax
    movl $1, %es:20(%di) # force a valid ACPI 3.x entry
    mov $24, %ecx # ask for 24 bytes
    int $0x15
    jc E820_f # carry set means "End of list already reached"
    mov $0x534D4150, %eax # some BIOSes trash this register
E820_jmpin:
    jcxz E820_skipent
    cmp $20, %cl
    jbe E820_notext
    testb $1, %es:20(%di) # is the "ignore this data" bit clear?
    je E820_skipent
E820_notext:
    mov %es:8(%di), %ecx # get lower uint32_t of memory region length
    or %es:12(%di), %ecx # or it with upper uint32_t to test for zero
    jz E820_skipent # if length uint64_t is 0, skip
    inc %bp
    add $24, %di
E820_skipent:
    test %ebx, %ebx # if ebx = 0, list is complete
    jne E820_lp
E820_f:
    mov %bp, %es:nmap_ent
    clc
    ret
E820_failed:
    stc
    ret

EnableA20:
    in $0x92, %al
    or $2, %al
    out %al, $0x92
    ret

EnablePM: # Set PE=1
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0
    ret

start:
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov $0x0500, %ax
    mov %ax, %ss
    mov $0x4FFF, %sp

    call DisableMotor
    call LoadFont
    call DisableCursor
    call DoE820

    lgdt GDT_PTR
    call EnableA20
    call EnablePM
    cli

    ljmp $CODE_SEG, $PmodeEntry
    jmp .

.code32

init_tss:
    # mov eax, tss_base
    # mov word [tss_entry + 2], ax # base (0-15)
    # shr eax, 16
    # mov byte [tss_entry + 4], al # base (16-23)
    # mov byte [tss_entry + 7], ah # base (24-31)
    movl $0x9F000, tss_base+4
    ret

PmodeEntry:
    mov $DATA_SEG, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov $0x9F000, %esp
    mov %ax, %ss

    call init_tss
    mov $TSS_SEG, %ax
    ltr %ax

    mov $0x07E00, %esi
    mov $0x100000, %edi
    mov $KERNEL_SIZE, %ecx
    rep movsl

    call EnableKRNL

    cli
    hlt

EnableKRNL:
    ljmp $CODE_SEG, $0x100000
    ret
