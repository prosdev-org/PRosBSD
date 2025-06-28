include ../cfg/Makefile.header

.PHONY: all clean keyboard vga_tty cmos

all: keyboard vga_tty cmos

keyboard:
	@make -C keyboard/
	@mv keyboard/*.o .

vga_tty:
	@make -C vga_tty/
	@mv vga_tty/*.o .

cmos:
	@make -C cmos/
	@mv cmos/*.o .

clean:
	@$(RM) *.o 