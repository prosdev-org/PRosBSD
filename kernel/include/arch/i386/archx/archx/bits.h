// Don't need an include guard, as this file can be included multiple times
#ifdef Archx_Bits_NEED_int8_t
typedef signed char int8_t;
#endif

#ifdef Archx_Bits_NEED_int16_t
typedef short int16_t;
#endif

#ifdef Archx_Bits_NEED_int32_t
typedef int int32_t;
#endif

#ifdef Archx_Bits_NEED_int64_t
typedef long long int64_t;
#endif

#ifdef Archx_Bits_NEED_uint8_t
typedef unsigned char uint8_t;
#endif

#ifdef Archx_Bits_NEED_uint16_t
typedef unsigned short uint16_t;
#endif

#ifdef Archx_Bits_NEED_uint32_t
typedef unsigned int uint32_t;
#endif

#ifdef Archx_Bits_NEED_uint64_t
typedef unsigned long long uint64_t;
#endif

#ifdef Archx_Bits_NEED_intmax_t
typedef long long intmax_t;
#endif

#ifdef Archx_Bits_NEED_uintmax_t
typedef unsigned long long uintmax_t;
#endif

#define Archx_Bits_ADDR int

#define Archx_Bits_long_MAX 0x7fffffffL
