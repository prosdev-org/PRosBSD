#ifndef MEMORY_VIRTUAL_LAYOUT_H
#define MEMORY_VIRTUAL_LAYOUT_H

#include <memory/virtual/paging.h>

#define MEM_VIRT_LAYOUT_NULL_PAGE_START 0x00000000
#define MEM_VIRT_LAYOUT_NULL_PAGE_END   0x00000FFF

#define MEM_VIRT_LAYOUT_USERLAND_START 0x00001000
#define MEM_VIRT_LAYOUT_USERLAND_END   0xBFFFFFFF

#define MEM_VIRT_LAYOUT_KERNEL_LOW_START 0xC0000000
#define MEM_VIRT_LAYOUT_KERNEL_LOW_END   0xC03FFFFF

#define MEM_VIRT_LAYOUT_KERNEL_HEAP_START 0xC0400000
#define MEM_VIRT_LAYOUT_KERNEL_HEAP_END   0xC13FFFFF

#define MEM_VIRT_LAYOUT_KERNEL_STACK_GUARD_START 0xC1400000
#define MEM_VIRT_LAYOUT_KERNEL_STACK_GUARD_END   0xC1400FFF

#define MEM_VIRT_LAYOUT_KERNEL_STACK_START 0xC1401000
#define MEM_VIRT_LAYOUT_KERNEL_STACK_END   0xC1402FF0

#define MEM_VIRT_LAYOUT_RESERVED_START 0xC1403000
#define MEM_VIRT_LAYOUT_RESERVED_END   0xFFBFFFFF

#define MEM_VIRT_LAYOUT_PAGE_MAPPING_START 0xFFC00000
#define MEM_VIRT_LAYOUT_PAGE_MAPPING_END   0xFFFFFFFF

// It is defined like this to prevent stack corruption
#define mem_virt_layout_setup_stack()                                                                \
    {                                                                                                \
        paging_alloc_and_map(MEM_VIRT_LAYOUT_KERNEL_STACK_START,                                     \
                             MEM_VIRT_LAYOUT_KERNEL_STACK_END - MEM_VIRT_LAYOUT_KERNEL_STACK_START); \
        __asm__ volatile("mov %0, %%esp" : : "r"(MEM_VIRT_LAYOUT_KERNEL_STACK_END));                 \
    }

#endif
