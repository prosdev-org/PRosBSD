#ifndef LIBK_STDARG_H
#define LIBK_STDARG_H

typedef __builtin_va_list va_list;

#define va_start(ap, count) __builtin_va_start(ap, count)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, T)       __builtin_va_arg(ap, T)
#define va_copy(dest, src)  __builtin_va_copy(dest, src)

#endif
