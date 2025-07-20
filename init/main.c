#include <extrns.h>
#include <generated/version.h>
#include <idt.h>
#include <keyboard.h>
#include <stdint.h>
#include <tty.h>
#include <console.h>

int main(void) {
    cleark();

    printf("Initializing idt...\n\r");
    idt_init();

    printf("Initializing keyboard...\n\r");
    keyboard_init();

    printf("Welcome to PRos32 v.%s!\n\r", VERSION_STRING);

    printf("Type 'help' for available commands.\n\r");
    console_init();

    // Main console loop
    for (;;) {
        console_process_input();
    }

    return 0;
}
