include cfg/Makefile.header

LDFLAGS += -Ttext 0

.PHONY: all clean boot init drivers

all: image

boot:
	@make -C boot/

init:
	@make -C init/

drivers:
	@make -C drivers/

kernel: boot init drivers
	@$(LD) $(LDFLAGS) --section-start=.text=0x7E00 -o boot/KERNEL.BIN boot/*.o init/*.o drivers/*.o
	@$(STRIP) boot/KERNEL.BIN
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
	@make clean -C drivers/
	@$(RM) fd.img