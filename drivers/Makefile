include ../cfg/Makefile.header

.PHONY: all clean keyboard vga_tty

all: keyboard vga_tty

keyboard:
	@make -C keyboard/
	@mv keyboard/*.o .

vga_tty:
	@make -C vga_tty/
	@mv vga_tty/*.o .

clean:
	@$(RM) *.o 