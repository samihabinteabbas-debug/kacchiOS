#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"


void memory_init(void);

/* Stack */
void* alloc_stack(void);
void  free_stack(void *addr);

/* Heap */
void* alloc_mem(uint32_t size);
void  free_mem(void *addr, uint32_t size);

#endif
