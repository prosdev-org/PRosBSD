#ifndef LIBK_STRING_V_H
#define LIBK_STRING_V_H

#include <string.h>
#include <unique/extern_c.h>

/* Set count bytes of volatile dest to ch */
EXTERN_C volatile void *memset_v(volatile void *dest, int ch, size_t count);
/* COPY count bytes of volatile src to dest */
EXTERN_C void *memcpy_v_src(void *dest, const volatile void *src, size_t count);
/* COPY count bytes of src to volatile dest */
EXTERN_C volatile void *memcpy_v_dest(volatile void *dest, const void *src, size_t count);

#endif
