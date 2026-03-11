#include <stdlib.h>

void *operator new(const size_t size) {
    return malloc(size);
}

void *operator new[](const size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
    free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

void operator delete(void *ptr, const size_t size) {
    (void) size;
    free(ptr);
}

void operator delete[](void *ptr, const size_t size) {
    (void) size;
    free(ptr);
}
