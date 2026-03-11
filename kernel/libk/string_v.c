#include <string_v.h>

volatile void *memset_v(volatile void *dest, const int ch, const size_t count) {
    volatile unsigned char *d = dest;

    for (size_t i = 0; i < count; i++) {
        d[i] = ch;
    }

    return dest;
}

void *memcpy_v_src(void *dest, const volatile void *src, const size_t count) {
    unsigned char *d = dest;
    const volatile unsigned char *s = src;

    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }

    return dest;
}

volatile void *memcpy_v_dest(volatile void *dest, const void *src, const size_t count) {
    volatile unsigned char *d = dest;
    const unsigned char *s = src;

    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }

    return dest;
}
