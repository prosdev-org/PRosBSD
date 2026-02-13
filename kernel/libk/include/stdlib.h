#ifndef LIBK_STDLIB_H
#define LIBK_STDLIB_H

#define Arch_Bits_NEED_size_t

#include <arch/bits.h>
#include <unique/public.h>

PUBLIC void *malloc(size_t size);
PUBLIC void free(void *ptr);

#endif
