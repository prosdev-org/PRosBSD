#include "cmos.h"
#include <cmos.h>
#include <extrns.h>
#include <io.h>
#include <stdint.h>

uint8_t cmos_read(uint8_t reg) {
    outb(reg, CMOS_ADDRESS_PORT);
    return inb(CMOS_DATA_PORT);
}

void cmos_write(uint8_t reg, uint8_t value) {
    outb(reg, CMOS_ADDRESS_PORT);
    outb(value, CMOS_DATA_PORT);
}

static uint8_t bcd_to_binary(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

void get_rtc_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours) {
    uint8_t regB = cmos_read(CMOS_STATUS_REGISTER_B);
    uint8_t bcd_format = !(regB & 0x04);

    *seconds = cmos_read(CMOS_RTC_SECONDS);
    *minutes = cmos_read(CMOS_RTC_MINUTES);
    *hours = cmos_read(CMOS_RTC_HOURS);

    if (bcd_format) {
        *seconds = bcd_to_binary(*seconds);
        *minutes = bcd_to_binary(*minutes);
        *hours = bcd_to_binary(*hours);
    }

    if (*seconds > 59 || *minutes > 59 || *hours > 23) {
        panic("cmos: invalid time values");
    }
}

void get_rtc_date(uint8_t *day, uint8_t *month, uint8_t *year) {
    uint8_t regB = cmos_read(CMOS_STATUS_REGISTER_B);
    uint8_t bcd_format = !(regB & 0x04);

    *day = cmos_read(CMOS_RTC_DAY_OF_MONTH);
    *month = cmos_read(CMOS_RTC_MONTH);
    *year = cmos_read(CMOS_RTC_YEAR);

    if (bcd_format) {
        *day = bcd_to_binary(*day);
        *month = bcd_to_binary(*month);
        *year = bcd_to_binary(*year);
    }

    if (*day == 0 || *day > 31 || *month == 0 || *month > 12) {
        panic("cmos: invalid date values");
    }
}
