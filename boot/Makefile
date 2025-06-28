include ../cfg/Makefile.header

.PHONY: all clean

all: MBR SETUP KERNEL_ENTRY

MBR:
	@$(ASM) -fbin MBR.asm -o MBR.BIN

SETUP:
	@$(ASM) -fbin setup.asm -o SETUP.BIN

KERNEL_ENTRY:
	@$(ASM) -felf32 kernel.asm -o KERNEL_ENTRY.o

clean:
	@$(RM) KERNEL_ENTRY.o KERNEL.BIN MBR.BIN SETUP.BIN