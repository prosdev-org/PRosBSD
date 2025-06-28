#include <extrns.h>

int main() {
    keyboard_init();
    cleark();
    putsk("Main protected mode kernel is loaded!");

    while (1) { // placheholder!
        char b = getchar();
        putck(b);
    }
    return 0;
}