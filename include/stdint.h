// part of x16-pros-libc

#ifndef STDINT_H
#define STDINT_H

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

typedef int intptr_t;
typedef unsigned int uintptr_t;

#define INT8_MAX  127
#define INT8_MIN  (-INT8_MAX - 1)
#define UINT8_MAX 255

#define INT16_MAX  32767
#define INT16_MIN  (-INT16_MAX - 1)
#define UINT16_MAX 65535

#define INT32_MAX  2147483647
#define INT32_MIN  (-INT32_MAX - 1)
#define UINT32_MAX 4294967295U

#define INT64_MAX  9223372036854775807LL
#define INT64_MIN  (-INT64_MAX - 1)
#define UINT64_MAX 18446744073709551615ULL

#define INT32_C(x)  x
#define UINT32_C(x) x ## U
#define INT64_C(x)  x ## LL
#define UINT64_C(x) x ## ULL

#endif //STDINT_H