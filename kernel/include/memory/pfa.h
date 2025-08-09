#ifndef MEMORY_PFA_H
#define MEMORY_PFA_H

void pfa_init();

uint32_t pf_alloc();

void pf_free(uint32_t address);

#endif
