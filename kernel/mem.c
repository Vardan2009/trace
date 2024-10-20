#include "mem.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t total_alloc;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

uint8_t phys_mem_bitmap[NUM_PAGE_FRAMES / 8];

static uint32_t page_dirs[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t page_dir_used[NUM_PAGES_DIRS];

void pmm_init(uint32_t mem_low, uint32_t mem_high) {
    page_frame_min =
        CEILDIV(mem_low, 0x1000);  // 1000 = 4096 : size of a single page
    page_frame_max = mem_high / 0x1000;
    total_alloc = 0;

    memset(phys_mem_bitmap, 0, sizeof(phys_mem_bitmap));
}

void init_memory(uint32_t mem_high, uint32_t phys_alloc_start) {
    initial_page_dir[0] = 0;
    invalidate(0);
    initial_page_dir[1023] = ((uint32_t)initial_page_dir - KERNEL_START) |
                             PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate(0xfffff000);

    pmm_init(phys_alloc_start, mem_high);
    memset(page_dirs, 0, 0x1000 * NUM_PAGES_DIRS);
    memset(page_dir_used, 0, NUM_PAGES_DIRS);
}

void invalidate(uint32_t vaddr) {
    asm volatile("invlpg %0" ::"m"(vaddr));
}