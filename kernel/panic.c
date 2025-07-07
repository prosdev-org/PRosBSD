#include <extrns.h>

volatile void panic(const char *s) {
    printf("Kernel panic: %s\n\r", s);
    for (;;)
        ;
}
