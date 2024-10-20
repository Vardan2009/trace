#include "lib/kmalloc.h"

static uint32_t heap_start;
static uint32_t heap_size;
static uint32_t threshold;
static bool kmalloc_initialized = false;

void init_kmalloc(uint32_t initial_heap_sz) {
    heap_start = KERNEL_MALLOC;
    heap_size = 0;
    threshold = 0;
    kmalloc_initialized = true;

    change_heap_size(initial_heap_sz);
}

void change_heap_size(int new_sz) {
    int old_page_top = CEILDIV(heap_size, 0x1000);
    int new_page_top = CEILDIV(new_sz, 0x1000);

    int diff = new_page_top - old_page_top;

    for (int i = 0; i < diff; i++) {
        uint32_t phys = pmm_alloc_page_frame();
        mem_map_page(KERNEL_MALLOC + old_page_top * 0x1000 + i * 0x1000, phys,
                     PAGE_FLAG_WRITE);
    }
}