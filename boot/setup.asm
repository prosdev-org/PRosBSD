[BITS 16]
[org 0x0600]

jmp start

GDT:
    dq 0
    
    ; ring 0, code (base=0, limit=0xFFFFF)
    dw 0xFFFF       ; limit (0-15)
    dw 0x0000       ; base (0-15)
    db 0x00         ; base (16-23)
    db 0x9A         ; P=1, DPL=00, S=1, Type=1010 (Exec/Read)
    db 0xCF         ; G=1, D/B=1, AVL=0, limit (16-19)=1111
    db 0x00         ; base (24-31)
    
    ; ring 0, data (base=0, limit=0xFFFFF)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92         ; P=1, DPL=00, S=1, Type=0010 (Read/Write)
    db 0xCF
    db 0x00
    
    ; ring 3, code
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA         ; P=1, DPL=11, S=1, Type=1010
    db 0xCF
    db 0x00
    
    ; ring 3, data
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2         ; P=1, DPL=11, S=1, Type=0010
    db 0xCF
    db 0x00
    
tss_entry:
    dw 0x0068 ; limit (104)
    dw tss_base ; base (0-15)
    db 0x00 ; base (16-23)
    db 0x89 ; P=1, DPL=00, Type=1001 (32-bit TSS available)
    db 0x00 ; G=0, AVL=0, limit (16-19)=0000
    db 0x00 ; base (24-31)

END_GDT:

GDT_PTR: 
    dw END_GDT - GDT - 1
    dd GDT

IDT_START:
    times 256 dq 0      ; full blank IDT
IDT_END:

IDT_PTR:
    dw IDT_END - IDT_START - 1
    dd IDT_START

tss_base:
    dd 0x00000000 ; last TSS
    dd 0x001FFFF0 ; ESP0
    dd DATA_SEG   ; SS0
    dd 0x00000000 ; ESP1
    dd 0x00000000 ; SS1
    dd 0x00000000 ; ESP2
    dd 0x00000000 ; SS2
    dd 0x00000000 ; CR3
    dd 0x00000000 ; EIP
    dd 0x00000000 ; EFLAGS
    dd 0x00000000 ; EAX
    dd 0x00000000 ; ECX
    dd 0x00000000 ; EDX
    dd 0x00000000 ; EBX
    dd 0x00000000 ; ESP
    dd 0x00000000 ; EBP
    dd 0x00000000 ; ESI
    dd 0x00000000 ; EDI
    dd 0x00000000 ; ES
    dd 0x00000000 ; CS
    dd 0x00000000 ; SS
    dd 0x00000000 ; DS
    dd 0x00000000 ; FS
    dd 0x00000000 ; GS
    dd 0x00000000 ; LDTR
    dw 0x0000 ; debug flag
    dw 0x0000 ; I/O Permission Bitmap offset

CODE_SEG equ 0x08 ; Ring 0
DATA_SEG equ 0x10 ; Ring 0
USER_CODE_SEG equ 0x18 | 3 ; Ring 3
USER_DATA_SEG equ 0x20 | 3 ; Ring 3
TSS_SEG equ 0x28

DisableMotor: ; FLoppy motor killer
    push dx
    mov dx, 0x3F2
    mov al, 0
    outsb
    pop dx
    ret

DisableCursor:
    mov ah, 0x01
    mov ch, 0x3F
    int 0x10
    ret

EnableA20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

EnablePM: ; Set PE=1
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax
	ret

start:
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000
	mov	ss, ax
	mov	sp, 0xFFFF
	
	call DisableMotor
    call DisableCursor

	lgdt [GDT_PTR]
    lidt [IDT_PTR]
    call EnableA20
	call EnablePM
	cli

	jmp CODE_SEG:PmodeEntry
	jmp $

[BITS 32]

init_tss:
    ; mov eax, tss_base
    ; mov word [tss_entry + 2], ax ; base (0-15)
    ; shr eax, 16
    ; mov byte [tss_entry + 4], al ; base (16-23)
    ; mov byte [tss_entry + 7], ah ; base (24-31)
    mov dword [tss_base + 4], 0x9F000
    ret

PmodeEntry:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x9F000
    mov ss, ax

    call init_tss
    mov ax, TSS_SEG
    ltr ax

    call setup_paging
    call EnableKRNL
	
	cli
	hlt

setup_paging:
    mov edi, 0x20000
    mov ecx, 4096        ; 4KB (1 page) - Page Directory
    xor eax, eax
    cld
    rep stosd

    mov edi, 0x21000
    mov ecx, 4096        ; 4KB (1 page) - Page Table 1
    rep stosd

    mov edi, 0x22000
    mov ecx, 4096        ; 4KB (1 page) - Page Table 2
    rep stosd

    mov edi, 0x23000
    mov ecx, 4096        ; 4KB (1 page) - Page Table 3
    rep stosd

    mov edi, 0x24000
    mov ecx, 4096        ; 4KB (1 page) - Page Table 4
    rep stosd

    mov dword [0x20000], 0x21007

    mov edi, 0x21000
    mov eax, 0x00000007
.pte_loop:
    stosd
    add eax, 0x1000 ; increment physical address by 4KB
    cmp eax, 0x400000
    jl .pte_loop

    mov eax, 0x20000
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000 ; set the PG bit
    mov cr0, eax

    ret


EnableKRNL:
    jmp CODE_SEG:0x7E00
    ret