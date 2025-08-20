include ../../cfg/Makefile.header

BUILD_DIRECTORY := ../../$(BUILD_DIRECTORY)/kernel/boot/

.PHONY: all

all: MBR SETUP KERNEL_ENTRY

MBR:
	@echo "$(ESC_GREEN)Compiling MBR (stage 1) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) MBR.asm -o $(BUILD_DIRECTORY)/MBR.o
	@$(LD) $(LDFLAGS) -Ttext 0x7C00 -o $(BUILD_DIRECTORY)/MBR.BIN $(BUILD_DIRECTORY)/MBR.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIRECTORY)/MBR.BIN

SETUP:
	@echo "$(ESC_GREEN)Compiling setup (stage 2) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) setup.asm -o $(BUILD_DIRECTORY)/SETUP.o
	@$(LD) $(LDFLAGS) -Ttext 0x0600 -o $(BUILD_DIRECTORY)/SETUP.BIN $(BUILD_DIRECTORY)/SETUP.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIRECTORY)/SETUP.BIN

KERNEL_ENTRY:
	@echo "$(ESC_GREEN)Compiling 32-bit kernel entry..$(ESC_END)"
	@$(AS) $(ASFLAGS) kernel.asm -o $(BUILD_DIRECTORY)/KERNEL_ENTRY.o
