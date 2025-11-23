// Don't need an include guard, as this file can be included multiple times
#include <libarch/bits.h>

#ifdef ARCH_BITS_NEED_NULL

#if __cplusplus >= 201103L
#define NULL nullptr
#elif defined(__cplusplus)
#define NULL 0L
#else
#define NULL ((void *) 0)
#endif

#endif

#ifdef ARCH_BITS_NEED_size_t
typedef unsigned ARCH_BITS_addr size_t;
#endif

#ifdef ARCH_BITS_NEED_ptrdiff_t
typedef ARCH_BITS_addr ptrdiff_t;
#endif

#ifdef ARCH_BITS_NEED_intptr_t
typedef ARCH_BITS_addr intptr_t;
#endif

#ifdef ARCH_BITS_NEED_uintptr_t
typedef unsigned ARCH_BITS_addr uintptr_t;
#endif

#ifdef ARCH_BITS_NEED_offsetof
#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t) ((char *) &(((type *) 0)->member) - (char *) 0))
#endif

#endif
