#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>

uint8_t cmos_read(uint8_t reg);
void cmos_write(uint8_t reg, uint8_t value);
void get_rtc_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours);
void get_rtc_date(uint8_t *day, uint8_t *month, uint8_t *year);

#endif // CMOS_H
