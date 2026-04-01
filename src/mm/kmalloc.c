#include "kmalloc.h"
#include "pmm.h"
#include <stdint.h>

// A simple bump allocator for the kernel heap
static uint8_t* heap_base = NULL;
static size_t heap_offset = 0;
static size_t heap_capacity = 0;

void* kmalloc(size_t size) {
    // Align allocations to 8 bytes for CPU efficiency
    size_t aligned_size = (size + 7) & ~7;

    if (heap_base == NULL || heap_offset + aligned_size > heap_capacity) {
        // We need more memory! Ask PMM for 16 pages (64KB) at a time
        size_t pages_needed = (aligned_size / PAGE_SIZE) + 16;
        heap_base = (uint8_t*)pmm_alloc(pages_needed);
        heap_capacity = pages_needed * PAGE_SIZE;
        heap_offset = 0;
    }

    void* ptr = heap_base + heap_offset;
    heap_offset += aligned_size;
    
    // Optional: Zero out the memory before returning it
    for(size_t i = 0; i < size; i++) {
        ((uint8_t*)ptr)[i] = 0;
    }

    return ptr;
}