include ../cfg/Makefile.header

.PHONY: all

all: MBR SETUP KERNEL_ENTRY

MBR:
	@echo "\033[0;32mCompiling MBR (stage 1) of fd.img..\033[0m"
	@$(ASM) -fbin MBR.asm -o MBR.BIN

SETUP:
	@echo "\033[0;32mCompiling setup (stage 2) of fd.img..\033[0m"
	@$(ASM) -fbin setup.asm -o SETUP.BIN

KERNEL_ENTRY:
	@echo "\033[0;32mCompiling 32-bit kernel entry..\033[0m"
	@$(ASM) -felf32 kernel.asm -o KERNEL_ENTRY.o