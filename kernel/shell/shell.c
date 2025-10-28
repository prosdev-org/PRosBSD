#include <stdio.h>

_Noreturn void shell_loop() {
    for (;;) {
        printf("\033[1;32m~$\033[0m ");
        char str[128];
        gets_s(str, sizeof(str) / sizeof(*str));
        if (*str) {
            printf("%s: command not found\n", str);
        }
    }
}
