#ifndef LIBK_STRING_H
#define LIBK_STRING_H

#define Arch_Bits_NEED_NULL
#define Arch_Bits_NEED_size_t

#include <arch/bits.h>
#include <unique/extern_c.h>
#include <unique/restrict.h>

EXTERN_C char *strcpy(char *RESTRICT dest, const char *RESTRICT src);
EXTERN_C char *strncpy(char *RESTRICT dest, const char *RESTRICT src, size_t count);
EXTERN_C char *strcat(char *RESTRICT dest, const char *RESTRICT src);
EXTERN_C char *strncat(char *RESTRICT dest, const char *RESTRICT src, size_t count);

EXTERN_C size_t strlen(const char *str);
EXTERN_C size_t strnlen(const char *str, size_t count);
EXTERN_C int strcmp(const char *lhs, const char *rhs);
EXTERN_C int strncmp(const char *lhs, const char *rhs, size_t count);
EXTERN_C char *strchr(const char *str, int ch);
EXTERN_C char *strrchr(const char *str, int ch);

EXTERN_C void *memchr(const void *ptr, int ch, size_t count);
EXTERN_C int memcmp(const void *lhs, const void *rhs, size_t count);
EXTERN_C void *memset(void *dest, int ch, size_t count);
EXTERN_C void *memcpy(void *RESTRICT dest, const void *RESTRICT src, size_t count);
EXTERN_C void *memmove(void *dest, const void *src, size_t count);

#endif
