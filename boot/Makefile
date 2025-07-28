include ../cfg/Makefile.header

.PHONY: all

all: MBR SETUP KERNEL_ENTRY

MBR:
	@echo "$(ESC_GREEN)Compiling MBR (stage 1) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) MBR.asm -o ../$(BUILD_DIRECTORY)/boot/MBR.o
	@$(LD) $(LDFLAGS) -Ttext 0x7C00 -o ../$(BUILD_DIRECTORY)/boot/MBR.BIN ../$(BUILD_DIRECTORY)/boot/MBR.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) ../$(BUILD_DIRECTORY)/boot/MBR.BIN

SETUP:
	@echo "$(ESC_GREEN)Compiling setup (stage 2) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) setup.asm -o ../$(BUILD_DIRECTORY)/boot/SETUP.o
	@$(LD) $(LDFLAGS) -Ttext 0x0600 -o ../$(BUILD_DIRECTORY)/boot/SETUP.BIN ../$(BUILD_DIRECTORY)/boot/SETUP.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) ../$(BUILD_DIRECTORY)/boot/SETUP.BIN

KERNEL_ENTRY:
	@echo "$(ESC_GREEN)Compiling 32-bit kernel entry..$(ESC_END)"
	@$(AS) $(ASFLAGS) kernel.asm -o ../$(BUILD_DIRECTORY)/boot/KERNEL_ENTRY.o
