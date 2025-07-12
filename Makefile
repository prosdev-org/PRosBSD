include cfg/Makefile.header

CFLAGS += -I./include 
LDFLAGS += -Ttext 0

.PHONY: all clean boot init drivers fs kernel_ kernel image hdd_image

all: hdd_image

boot:
	@make -C boot/

init:
	@echo "\033[0;32mCompiling init..\033[0m"
	@$(CC) $(CFLAGS) -c init/main.c -o init/init.o

drivers:
	@for f in $(wildcard drivers/cmos/*.c); do echo "\033[0;32mCompiling CMOS driver..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done
	@for f in $(wildcard drivers/keyboard/*.c); do echo "\033[0;32mCompiling keyboard driver..\033[ \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done
	@for f in $(wildcard drivers/vga_tty/*.c); do echo "\033[0;32mCompiling VGA TTY driver..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done
	@for f in $(wildcard drivers/mouse/*.c); do echo "\033[0;32mCompiling mouse driver..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done
	@for f in $(wildcard drivers/pata_pio/*.c); do echo "\033[0;32mCompiling PATA PIO driver..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done

fs:
	@for f in $(wildcard fs/FATs/FAT32/*.c); do echo "\033[0;32mCompiling FAT32 FS..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done

kernel_:
	@for f in $(wildcard kernel/*.c); do echo "\033[0;32mCompiling kernel piece..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done
	@for f in $(wildcard kernel/memory/*.c); do echo "\033[0;32mCompiling memory operations..\033[0m \033[34m$$f\033[0m"; $(CC) -c $(CFLAGS) $$f; done


kernel: boot init drivers fs kernel_
	@$(LD) $(LDFLAGS) --section-start=.text=0x7E00 -o boot/KERNEL_.BIN boot/KERNEL_ENTRY.o init/*.o *.o
	@cp boot/KERNEL_.BIN boot/KERNEL.BIN
	@$(STRIP) boot/KERNEL.BIN
	@$(OBJCOPY) $(OBJCOPYFLAGS) boot/KERNEL.BIN

image: kernel
	@dd if=/dev/zero of=$(IMAGE_NAME) count=2880 bs=512
	@mkfs.fat -F 12 $(IMAGE_NAME)
	@dd if=boot/MBR.BIN of=$(IMAGE_NAME) conv=notrunc
	@mcopy -i $(IMAGE_NAME) boot/SETUP.BIN ::/
	@mcopy -i $(IMAGE_NAME) boot/KERNEL.BIN ::/

hdd_image: image
	@make unmount_image
	@dd if=/dev/zero of=$(HDD_IMAGE) bs=1M count=70
	@parted -s $(HDD_IMAGE) mklabel msdos
	@parted -s $(HDD_IMAGE) mkpart primary 1MiB 34MiB
	@parted -s $(HDD_IMAGE) mkpart primary 35MiB 69MiB
	@sudo kpartx -av $(HDD_IMAGE)
	@sleep 2
	@sudo mkfs.fat -F 32 /dev/mapper/loop0p1
	@sudo mkfs.ext2 /dev/mapper/loop0p2
	@sudo mcopy -i /dev/mapper/loop0p1 $(MEMDISK) ::/
	@sudo mcopy -i /dev/mapper/loop0p1 $(IMAGE_NAME) ::/fd.img
	@sudo mkdir -p $(GRUB_MOUNT)
	@sudo mount /dev/mapper/loop0p1 $(GRUB_MOUNT)
	@sudo mkdir -p $(GRUB_MOUNT)/boot/grub/
	@sudo cp cfg/grub.cfg $(GRUB_MOUNT)/boot/grub/grub.cfg
	@sudo grub-install \
		--target=i386-pc \
		--boot-directory=$(GRUB_MOUNT)/boot \
		--modules="part_msdos fat" \
		--recheck \
		--force \
		/dev/loop0
	@make unmount_image
	@parted -s $(HDD_IMAGE) unit B print

unmount_image:
	-@sudo umount $(GRUB_MOUNT)
	-@sudo $(RM) $(GRUB_MOUNT)
	-@sudo kpartx -d /dev/loop0
	-@sudo losetup -d /dev/loop0

format:
	@find . -name '*.h' -o -name '*.c' | xargs clang-format -i

clean:
	@$(RM) $(IMAGE_NAME) $(HDD_IMAGE)
	@find . -type f \( -name "*.o" -o -name "*.BIN" \) -exec rm -f {} +
