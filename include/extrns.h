#ifndef EXTRNS_H
#define EXTRNS_H
#include <stdint.h>

// keyboard driver
void keyboard_init();
char getchar();

// tty driver
void cleark();
void putck(char c);
void putsk(const char *str);

// cmos driver
uint8_t cmos_read(uint8_t reg);
void cmos_write(uint8_t reg, uint8_t value);
void get_rtc_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours);
void get_rtc_date(uint8_t *day, uint8_t *month, uint8_t *year);

#endif // EXTRNS_H