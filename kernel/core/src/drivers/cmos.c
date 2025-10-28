#include <cpu/io.h>
#include <drivers/cmos.h>
#include <panic.h>
#include <stdint.h>

#define CMOS_ADDRESS_PORT      0x70
#define CMOS_DATA_PORT         0x71
#define CMOS_RTC_SECONDS       0x00
#define CMOS_RTC_MINUTES       0x02
#define CMOS_RTC_HOURS         0x04
#define CMOS_RTC_DAY_OF_WEEK   0x06
#define CMOS_RTC_DAY_OF_MONTH  0x07
#define CMOS_RTC_MONTH         0x08
#define CMOS_RTC_YEAR          0x09
#define CMOS_RTC_CENTURY       0x32
#define CMOS_STATUS_REGISTER_A 0x0A
#define CMOS_STATUS_REGISTER_B 0x0B

uint8_t cmos_read(const uint8_t reg) {
    outb(reg, CMOS_ADDRESS_PORT);
    return inb(CMOS_DATA_PORT);
}

void cmos_write(const uint8_t reg, const uint8_t value) {
    outb(reg, CMOS_ADDRESS_PORT);
    outb(value, CMOS_DATA_PORT);
}

static uint8_t bcd_to_binary(const uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

void get_rtc_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours) {
    const uint8_t regB = cmos_read(CMOS_STATUS_REGISTER_B);
    const uint8_t bcd_format = !(regB & 0x04);

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
    const uint8_t regB = cmos_read(CMOS_STATUS_REGISTER_B);
    const uint8_t bcd_format = !(regB & 0x04);

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
