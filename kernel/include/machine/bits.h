// Don't need an include guard, as this file can be included multiple times
#include <archx/bits.h>

#ifdef Archx_Bits_NEED_NULL
#define NULL ((void *) 0)
#endif

#ifdef Archx_Bits_NEED_size_t

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ Archx_Bits_ADDR
#endif
typedef __SIZE_TYPE__ size_t;

#endif

#ifdef Archx_Bits_NEED_ptrdiff_t
typedef Archx_Bits_ADDR ptrdiff_t;
#endif

#ifdef Archx_Bits_NEED_intptr_t
typedef Archx_Bits_ADDR intptr_t;
#endif

#ifdef Archx_Bits_NEED_uintptr_t
typedef unsigned Archx_Bits_ADDR uintptr_t;
#endif

#ifdef Archx_Bits_NEED_offsetof
#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t) ((char *) &(((type *) 0)->member) - (char *) 0))
#endif

#endif
