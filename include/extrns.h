#ifndef EXTRNS_H
#define EXTRNS_H
#include <stdarg.h>
#include <stddef.h>

/////////

int printf(const char *restrict format, ...);
int sprintf(char *restrict str, const char *restrict format, ...);
int snprintf(char *restrict str, size_t size, const char *restrict format, ...);
int vprintf(const char *restrict format, const va_list ap);
int vsprintf(char *restrict buf, const char *restrict format, const va_list args);
int vsnprintf(char *restrict str, const size_t size, const char *restrict format, va_list ap);

volatile void panic(const char *s);
void *mallocl(size_t size);
void *malloc(size_t size);
void free(void *ptr);

#endif // EXTRNS_H
