#include <extrns.h>
#include <stdbool.h>
#include <stdint.h>

int main() {
    keyboard_init();
    cleark();
    printf("Main protected mode kernel is loaded!\n");

    char c;
    while (true) {
        putck(getchar());
    }

    return 0;
}