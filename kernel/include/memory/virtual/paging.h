#ifndef PAGING_H
#define PAGING_H

#include <stddef.h>
#include <stdint.h>

void *paging_addr_phys_to_virt(uintptr_t phys);

void paging_init();

void paging_map(uintptr_t phys, uintptr_t virt);

void paging_alloc_and_map(uintptr_t base, size_t size);

#endif
