export CC = clang
export CFLAGS = -static -O3 -Wall -Wextra

SUBS := $(dir $(wildcard bin/*/Makefile sbin/*/Makefile))
.PHONY: all clean  $(SUBS)

all: $(SUBS)

$(SUBS):
	@$(MAKE) -C $@

clean:
	@for dir in $(SUBS); do $(MAKE) -C $$dir clean; done