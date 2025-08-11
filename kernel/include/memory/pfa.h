#ifndef MEMORY_PFA_H
#define MEMORY_PFA_H

#include <stdint.h>

void pfa_init();

uint32_t pf_alloc();

void pf_free(uint32_t idx);

#endif
