#include <drivers/idt.h>
#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <generated/version.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    cleark();

    printf("Initializing idt...\n\r");
    idt_init();

    printf("Initializing keyboard...\n\r");
    keyboard_init();

    printf("Welcome to PRos32 v.%s!\n\r", VERSION_STRING);

    for (;;) {
        putck(getchark());
    }

    return 0;
}
