// Don't need an include guard, as this file can be included multiple times
#include <xarch/bits.h>

#ifdef Arch_Bits_NEED_NULL

#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#else
#define NULL ((void *) 0)
#endif

#endif

#ifdef Arch_Bits_NEED_size_t
typedef unsigned Arch_Bits_ADDR size_t;
#endif

#ifdef Arch_Bits_NEED_ptrdiff_t
typedef Arch_Bits_ADDR ptrdiff_t;
#endif

#ifdef Arch_Bits_NEED_intptr_t
typedef Arch_Bits_ADDR intptr_t;
#endif

#ifdef Arch_Bits_NEED_uintptr_t
typedef unsigned Arch_Bits_ADDR uintptr_t;
#endif

#ifdef Arch_Bits_NEED_offsetof
#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t) ((char *) &(((type *) 0)->member) - (char *) 0))
#endif

#endif
