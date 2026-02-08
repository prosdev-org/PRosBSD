#ifndef LIBK_STRING_H
#define LIBK_STRING_H

#define ARCH_BITS_NEED_NULL
#define ARCH_BITS_NEED_size_t

#include <arch/bits.h>
#include <unique/public.h>

PUBLIC char *strcpy(char *restrict dest, const char *restrict src);
PUBLIC char *strncpy(char *restrict dest, const char *restrict src, size_t count);
PUBLIC char *strcat(char *restrict dest, const char *restrict src);
PUBLIC char *strncat(char *restrict dest, const char *restrict src, size_t count);

PUBLIC size_t strlen(const char *str);
PUBLIC size_t strnlen(const char *str, size_t count);
PUBLIC int strcmp(const char *lhs, const char *rhs);
PUBLIC int strncmp(const char *lhs, const char *rhs, size_t count);
PUBLIC char *strchr(const char *str, int ch);
PUBLIC char *strrchr(const char *str, int ch);

PUBLIC void *memchr(const void *ptr, int ch, size_t count);
PUBLIC int memcmp(const void *lhs, const void *rhs, size_t count);
PUBLIC void *memset(void *dest, int ch, size_t count);
PUBLIC void *memcpy(void *restrict dest, const void *restrict src, size_t count);
PUBLIC void *memmove(void *dest, const void *src, size_t count);

#endif
