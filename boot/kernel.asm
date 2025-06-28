[BITS 32]

section .multiboot
	align 4
	dd 0x1BADB002
	dd 0x00
	dd - (0x1BADB002 + 0x00)

_start:
    call main
    ret

global _start
extern main