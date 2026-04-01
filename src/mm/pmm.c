#include "pmm.h"
#include "limine.h"

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

static uint8_t* bitmap = NULL;
static size_t bitmap_size = 0;
static size_t highest_page = 0;
static uint64_t hhdm_offset = 0;

// Helper: Set a bit in the bitmap
static void bitmap_set(size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

// Helper: Clear a bit in the bitmap
static void bitmap_clear(size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

// Helper: Test a bit
static int bitmap_test(size_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(void) {
    struct limine_memmap_response* memmap = memmap_request.response;
    hhdm_offset = hhdm_request.response->offset;

    // 1. Find the highest physical address to size our bitmap
    uint64_t top_address = 0;
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t top = entry->base + entry->length;
            if (top > top_address) top_address = top;
        }
    }

    highest_page = top_address / PAGE_SIZE;
    bitmap_size = highest_page / 8;
    if (bitmap_size == 0) return;

    // 2. Find a place to store the bitmap itself
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size) {
            bitmap = (uint8_t*)(entry->base + hhdm_offset);
            break;
        }
    }

    // 3. Initialize everything as "used" (1)
    for (size_t i = 0; i < bitmap_size; i++) bitmap[i] = 0xFF;

    // 4. Mark usable regions as "free" (0)
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE) {
                bitmap_clear((entry->base + j) / PAGE_SIZE);
            }
        }
    }

    // Mark the memory used by the bitmap itself as used!
    uint64_t bitmap_phys = (uint64_t)bitmap - hhdm_offset;
    for (uint64_t j = 0; j < bitmap_size; j += PAGE_SIZE) {
        bitmap_set((bitmap_phys + j) / PAGE_SIZE);
    }
}

void* pmm_alloc(size_t pages) {
    size_t contiguous_free = 0;
    size_t start_bit = 0;

    for (size_t i = 0; i < highest_page; i++) {
        if (!bitmap_test(i)) {
            if (contiguous_free == 0) start_bit = i;
            contiguous_free++;
            if (contiguous_free == pages) {
                // Found enough pages! Mark them used.
                for (size_t j = start_bit; j < start_bit + pages; j++) {
                    bitmap_set(j);
                }
                return (void*)((start_bit * PAGE_SIZE) + hhdm_offset);
            }
        } else {
            contiguous_free = 0;
        }
    }
    return NULL; // Out of memory
}

void pmm_free(void* ptr, size_t pages) {
    uint64_t phys_addr = (uint64_t)ptr - hhdm_offset;
    size_t start_bit = phys_addr / PAGE_SIZE;
    for (size_t i = start_bit; i < start_bit + pages; i++) {
        bitmap_clear(i);
    }
}