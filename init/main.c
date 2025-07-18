#include <extrns.h>
#include <generated/version.h>
#include <idt.h>
#include <keyboard.h>
#include <stdint.h>
#include <tty.h>

int main(void) {
    cleark();

    printf("Initializing idt...\n\r");
    idt_init();

    printf("Initializing keyboard...\n\r");
    keyboard_init();

    printf("Welcome to PRos32 v.%s!\n\r", VERSION_STRING);

    for (;;) {
        putck(getchar());
    }

    return 0;
}
