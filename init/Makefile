include ../cfg/Makefile.header

CFLAGS += -I../include
LDFLAGS += -Ttext 0

.PHONY: all clean

all: main

main:
	@$(CC) $(CFLAGS) -c main.c -o init.o

clean:
	@$(RM) init.o