include ../cfg/Makefile.header

CFLAGS += -I../include/ -I./include/ -I./libk/include/
LDFLAGS += -T ../cfg/linker.ld
BUILD_DIRECTORY := ../$(BUILD_DIRECTORY)/kernel

.PHONY: all boot objs kernel

all: kernel

boot:
	@mkdir -p $(BUILD_DIRECTORY)/boot/
	@make -C boot/

objs:
	@for f in $$(find . -name "*.c" -not -path "./boot/*"); \
		do \
		echo "$(ESC_GREEN)Compiling kernel.. $(ESC_END) $(ESC_BLUE)$$f$(ESC_END)"; \
		mkdir -p $(BUILD_DIRECTORY)/$$(dirname $$f); \
		$(CC) $(CFLAGS) $$f -o $(BUILD_DIRECTORY)/$$(dirname $$f)/$$(basename $$f .c).o; \
	done;

	@for f in $$(find . -name "*.asm" -not -path "./boot/*"); \
		do \
		echo "$(ESC_GREEN)Assembling kernel.. $(ESC_END) $(ESC_BLUE)$$f$(ESC_END)"; \
		mkdir -p $(BUILD_DIRECTORY)/$$(dirname $$f); \
		$(AS) $(ASFLAGS) $$f -o $(BUILD_DIRECTORY)/$$(dirname $$f)/$$(basename $$f .asm).o; \
	done;

kernel: boot objs
	@$(LD) $(LDFLAGS) -o $(BUILD_DIRECTORY)/boot/KERNEL_.BIN $(BUILD_DIRECTORY)/boot/KERNEL_ENTRY.o $$(find $(BUILD_DIRECTORY)/ -name "*.o" -not -path "$(BUILD_DIRECTORY)/boot/*")
	@cp $(BUILD_DIRECTORY)/boot/KERNEL_.BIN $(BUILD_DIRECTORY)/KERNEL.BIN
	@$(STRIP) $(BUILD_DIRECTORY)/KERNEL.BIN
	@$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIRECTORY)/KERNEL.BIN
