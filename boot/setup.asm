[BITS 16]
[org 0x0600]

jmp start

GDT: 
	; null
	dd 0                
	dd 0 
	
	; code
	dw 0xFFFF           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10011010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
	
	; data
	dw 0xFFFF           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10010010b        ; access
	db 11001111b        ; granularity
	db 0                ; base 

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


CODE_SEG equ 0x08
DATA_SEG equ 0x10

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
PmodeEntry:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0xA000
    mov ss, ax

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