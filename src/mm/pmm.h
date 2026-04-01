#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

void pmm_init(void);
void* pmm_alloc(size_t pages);
void pmm_free(void* ptr, size_t pages);

#endif