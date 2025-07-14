#include <extrns.h>
#include <keyboard.h>
#include <generated/version.h>


int main(void) {
    keyboard_init();
    cleark();
    
    printf("Welcome to PRos32 v.%s!", VERSION_STRING);
    getchar();

    return 0;
}
