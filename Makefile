include cfg/Makefile.header

LDFLAGS += -Ttext 0

.PHONY: all clean boot init drivers multiboot kernel image hdd_image

all: hdd_image

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

hdd_image: image
	@if [ ! -f "memdisk" ]; then \
		wget -O memdisk https://github.com/redox-os/isolinux/raw/refs/heads/master/memdisk; \
	fi
	@dd if=/dev/zero of=$(HDD_IMAGE) bs=1M count=35
	@parted -s $(HDD_IMAGE) mklabel msdos
	@parted -s $(HDD_IMAGE) mkpart primary 1MiB 8MiB
	@parted -s $(HDD_IMAGE) mkpart primary 9MiB 34MiB
	@sudo kpartx -av $(HDD_IMAGE)
	@sleep 2
	@sudo mkfs.fat -F 32 /dev/mapper/loop0p1
	@sudo mkfs.ext2 /dev/mapper/loop0p2
	@sudo mcopy -i /dev/mapper/loop0p1 memdisk ::/
	@sudo mcopy -i /dev/mapper/loop0p1 $(IMAGE_NAME) ::/fd.img
	@sudo mkdir -p $(GRUB_MOUNT)
	@sudo mount /dev/mapper/loop0p1 $(GRUB_MOUNT)
	@sudo mkdir -p $(GRUB_MOUNT)/boot/grub
	@echo 'set timeout=0\nset default=0\n\nmenuentry "Pros" {\n  insmod part_msdos\n  insmod fat\n  set root='"'(hd0,msdos1)'"'\n  linux16 /memdisk silent\n  initrd16 /fd.img\n  boot\n}' | sudo tee $(GRUB_MOUNT)/boot/grub/grub.cfg > /dev/null
	@sudo grub-install \
		--target=i386-pc \
		--boot-directory=$(GRUB_MOUNT)/boot \
		--modules="part_msdos fat" \
		--recheck \
		--force \
		/dev/loop0
	@sudo umount $(GRUB_MOUNT)
	@sudo $(RM) $(GRUB_MOUNT)
	@sudo kpartx -d $(HDD_IMAGE)
	@parted -s $(HDD_IMAGE) unit B print

clean:
	@make clean -C boot/
	@make clean -C init/
	@make clean -C drivers/
	@$(RM) $(IMAGE_NAME) $(HDD_IMAGE) memdisk