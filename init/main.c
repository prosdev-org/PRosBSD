#include <extrns.h>
#include <stdint.h>

uint8_t seconds, minutes, hours;
uint8_t day, month, year;

int main() {
    keyboard_init();
    cleark();
    putsk("Main protected mode kernel is loaded!");
    get_rtc_time(&seconds, &minutes, &hours);
    get_rtc_date(&day, &month, &year);

    putsk("Current RTC Time:");
    putck(hours / 10 + '0'); putck(hours % 10 + '0'); putck(':');
    putck(minutes / 10 + '0'); putck(minutes % 10 + '0'); putck(':');
    putck(seconds / 10 + '0'); putck(seconds % 10 + '0'); putck('\n');
    putsk("Current RTC Date:");
    putck(month / 10 + '0'); putck(month % 10 + '0'); putck('/');
    putck(day / 10 + '0'); putck(day % 10 + '0'); putck('/');
    putck(year / 10 + '0'); putck(year % 10 + '0'); putck('\n');
    
    while (1) { // placeholder!
        char b = getchar();
        putck(b);
    }
    return 0;
}