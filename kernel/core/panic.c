#include <stdio.h>

volatile void panic(const char *s) {
    printf("Kernel panic: %s\n\r", s);
    __asm__ volatile("cli\n"
                     "hlt\n"
                     :
                     :);
}
