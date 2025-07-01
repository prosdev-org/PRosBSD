#ifndef EXTRNS_H
#define EXTRNS_H
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

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

/////////

int printf(const char *restrict format, ...);
int sprintf(char *restrict str, const char *restrict format, ...);
int snprintf(char *restrict str, size_t size, const char *restrict format, ...);
int vprintf(const char *restrict format, const va_list ap);
int vsprintf(char *restrict buf, const char *restrict format, const va_list args);
int vsnprintf(char *restrict str, const size_t size, const char *restrict format, va_list ap);

volatile void panic(const char * s);
void* mallocl(size_t size);
void *malloc(size_t size);
void free(void *ptr);

#endif // EXTRNS_H