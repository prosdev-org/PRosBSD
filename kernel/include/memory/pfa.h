#ifndef MEMORY_PFA_H
#define MEMORY_PFA_H

#include <stdint.h>

void pfa_init();

uintptr_t pf_alloc();

void pf_free(uintptr_t addr);

#endif
