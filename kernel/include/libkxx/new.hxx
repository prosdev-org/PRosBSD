#ifndef LIBKXX_NEW_KXX
#define LIBKXX_NEW_KXX

#include <stddef.h>

inline void *operator new(const size_t, void *ptr) {
    return ptr;
}

inline void *operator new[](const size_t, void *ptr) {
    return ptr;
}

#endif
