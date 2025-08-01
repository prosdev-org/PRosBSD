#include <memory/map.h>
#include <stdint.h>

extern uint32_t __kernel_end;

void *get_kernel_end() {
    return (void *) &__kernel_end;
}
