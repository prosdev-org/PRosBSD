#include <extrns.h>
#include <mouse.h>
#include <stdbool.h>
#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main() {
    keyboard_init();
    mouse_init();
    cleark();

    printf("Main protected mode kernel is loaded!\n");

    int cursor_x = 400;
    int cursor_y = 300;

    char c;
    while (true) {
        mouse_poll();
        if (mouse.x != 0 || mouse.y != 0) {
            cursor_x += mouse.x;
            cursor_y += mouse.y;

            cursor_x = MAX(0, MIN(cursor_x, 799));
            cursor_y = MAX(0, MIN(cursor_y, 599));

            mouse.x = 0;
            mouse.y = 0;

            printf("Cursor at %d.%d.\n", cursor_x, cursor_y);
        }

        if (mouse.buttons & MOUSE_LEFT_BTN) {
            printf("Left button pressed at (%d, %d)\n", cursor_x, cursor_y);
        }

        if (mouse.buttons & MOUSE_RIGHT_BTN) {
            printf("Right button pressed at (%d, %d)\n", cursor_x, cursor_y);
        }

        if (mouse.z != 0) {
            printf("Scrolled: %d\n", mouse.z);
            mouse.z = 0;
        }
    }

    return 0;
}
