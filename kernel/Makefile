include ../cfg/Makefile.header

CFLAGS_KERNEL = $(CFLAGS) -I../include/ -I./include/ -I./libk/include/

.PHONY: all boot objs kernel

all: kernel

boot:
	@mkdir -p ../$(BUILD_DIRECTORY)/boot/
	@make -C boot/

objs:
	@for f in $$(find . -name "*.c" -not -path "./boot/*"); \
		do \
		echo "$(ESC_GREEN)Compiling kernel.. $(ESC_END) $(ESC_BLUE)$$f$(ESC_END)"; \
		mkdir -p ../$(BUILD_DIRECTORY)/kernel/$$(dirname $$f); \
		$(CC) -c $(CFLAGS_KERNEL) $$f -o ../$(BUILD_DIRECTORY)/kernel/$$(dirname $$f)/$$(basename $$f .c).o; \
	done;

	@for f in $$(find . -name "*.asm" -not -path "./boot/*"); \
		do \
		echo "$(ESC_GREEN)Assembling kernel.. $(ESC_END) $(ESC_BLUE)$$f$(ESC_END)"; \
		mkdir -p ../$(BUILD_DIRECTORY)/kernel/$$(dirname $$f); \
		$(AS) $(ASFLAGS) $$f -o ../$(BUILD_DIRECTORY)/kernel/$$(dirname $$f)/$$(basename $$f .asm).o; \
	done;

kernel: boot objs
	@$(LD) $(LDFLAGS) --section-start=.text=0x7E00 -o ../$(BUILD_DIRECTORY)/boot/KERNEL_.BIN ../$(BUILD_DIRECTORY)/boot/KERNEL_ENTRY.o $$(find ../$(BUILD_DIRECTORY)/kernel/ -name "*.o")
	@cp ../$(BUILD_DIRECTORY)/boot/KERNEL_.BIN ../$(BUILD_DIRECTORY)/kernel/KERNEL.BIN
	@$(STRIP) ../$(BUILD_DIRECTORY)/kernel/KERNEL.BIN
	@$(OBJCOPY) $(OBJCOPYFLAGS) ../$(BUILD_DIRECTORY)/kernel/KERNEL.BIN
