include ../cfg/Makefile.header

.PHONY: all

all: MBR SETUP KERNEL_ENTRY

MBR:
	@echo "$(ESC_GREEN)Compiling MBR (stage 1) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) MBR.asm -o MBR.o $(NULL)
	@$(LD) $(LDFLAGS) -Ttext 0x7C00 -o MBR.BIN MBR.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) MBR.BIN

SETUP:
	@echo "$(ESC_GREEN)Compiling setup (stage 2) of fd.img..$(ESC_END)"
	@$(AS) $(ASFLAGS) setup.asm -o SETUP.o $(NULL)
	@$(LD) $(LDFLAGS) -Ttext 0x0600 -o SETUP.BIN SETUP.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) SETUP.BIN

KERNEL_ENTRY:
	@echo "$(ESC_GREEN)Compiling 32-bit kernel entry..$(ESC_END)"
	@$(AS) $(ASFLAGS) kernel.asm -o KERNEL_ENTRY.o $(NULL)