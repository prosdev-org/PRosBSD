#include <stdint.h>
#include <extrns.h>

#define KERNEL_END     0x52900
#define STACK_TOP      0x9F000
#define HEAP_SAFE_GAP  0x1000

static uint32_t low_mem_ptr = KERNEL_END + HEAP_SAFE_GAP;

void* mallocl(size_t size) {
    low_mem_ptr = (low_mem_ptr + 15) & ~0xF;
    
    if (low_mem_ptr + size > STACK_TOP - 1024) {
        panic("mallocl: out of low memory");
    }
    
    void* ptr = (void*)low_mem_ptr;
    low_mem_ptr += size;
    return ptr;
}