#ifndef LIBK_STDLIB_H
#define LIBK_STDLIB_H

#define Arch_Bits_NEED_size_t

#include <arch/bits.h>
#include <unique/extern_c.h>

EXTERN_C void *malloc(size_t size);
EXTERN_C void free(void *ptr);

#endif
