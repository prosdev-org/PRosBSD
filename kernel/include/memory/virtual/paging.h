#ifndef PAGING_H
#define PAGING_H

void *paging_addr_phys_to_virt(uintptr_t phys);

void paging_init();

#endif
