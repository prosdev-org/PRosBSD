CFLAGS += -Wno-cast-function-type-mismatch #FSM uses (*state_func_t)(void);
TARGET = init.elf
SRCS = init.c states.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
