#ifndef LIBKXX_NEW_HXX
#define LIBKXX_NEW_HXX

#include <stddef.h>

inline void *operator new(const size_t, void *ptr) {
    return ptr;
}

inline void *operator new[](const size_t, void *ptr) {
    return ptr;
}

#endif
