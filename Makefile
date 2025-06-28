include cfg/Makefile.header

LDFLAGS += -Ttext 0

.PHONY: all clean boot init

all: image

boot:
	@make -C boot/

init:
	@make -C init/

kernel: boot init
	@$(LD) $(LDFLAGS) --section-start=.text=0x7E00 -o boot/KERNEL.BIN boot/KERNEL_ENTRY.o init/init.o
	@$(OBJCOPY) $(OBJCOPYFLAGS) boot/KERNEL.BIN

image: kernel
	@dd if=/dev/zero of=$(IMAGE_NAME) count=2880 bs=512
	@mkfs.fat -F 12 $(IMAGE_NAME)
	@dd if=boot/MBR.BIN of=$(IMAGE_NAME) conv=notrunc
	@mcopy -i $(IMAGE_NAME) boot/SETUP.BIN ::/
	@mcopy -i $(IMAGE_NAME) boot/KERNEL.BIN ::/

clean:
	@make clean -C boot/
	@make clean -C init/
	@$(RM) fd.img