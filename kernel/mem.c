#include "mem.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t total_alloc;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

uint8_t phys_mem_bitmap[NUM_PAGE_FRAMES / 8];

static uint32_t page_dirs[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t page_dir_used[NUM_PAGES_DIRS];

int mem_num_vpages;

void pmm_init(uint32_t mem_low, uint32_t mem_high) {
    page_frame_min =
        CEILDIV(mem_low, 0x1000);  // 1000 = 4096 : size of a single page
    page_frame_max = mem_high / 0x1000;
    total_alloc = 0;

    memset(phys_mem_bitmap, 0, sizeof(phys_mem_bitmap));
}

void init_memory(uint32_t mem_high, uint32_t phys_alloc_start) {
    mem_num_vpages = 0;
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

uint32_t *mem_get_current_page_dir() {
    uint32_t pd;
    asm volatile("mov %%cr3, %0" : "=r"(pd));
    pd += KERNEL_START;
    return (uint32_t *)pd;
}

void mem_change_page_dir(uint32_t *pd) {
    pd = (uint32_t *)(((uint32_t)pd) - KERNEL_START);
    asm volatile("mov %0, %%eax \n mov %%eax, %%cr3" ::"m"(pd));
}

void mem_map_page(uint32_t vaddr, uint32_t physaddr, uint32_t flags) {
    uint32_t *prev_page_dir = 0;
    if (vaddr >= KERNEL_START) {
        prev_page_dir = mem_get_current_page_dir();
        if (prev_page_dir != initial_page_dir) {
            mem_change_page_dir(initial_page_dir);
        }
    }

    uint32_t pd_idx = vaddr >> 22;
    uint32_t pt_idx = vaddr >> 12 & 0x3ff;

    uint32_t *page_dir = REC_PAGEDIR;
    uint32_t *pt = REC_PAGETABLE(pd_idx);

    if (!(page_dir[pd_idx] & PAGE_FLAG_PRESENT)) {
        uint32_t pt_paddr = pmm_alloc_page_frame();
        page_dir[pd_idx] = pt_paddr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE |
                           PAGE_FLAG_OWNER | flags;
        invalidate(vaddr);

        for (uint32_t i = 0; i < 1024; i++) pt[i] = 0;
    }

    pt[pt_idx] = physaddr | PAGE_FLAG_PRESENT | flags;
    ++mem_num_vpages;
    invalidate(vaddr);

    if (prev_page_dir != 0) {
        sync_page_dirs();
        if (prev_page_dir != initial_page_dir)
            mem_change_page_dir(prev_page_dir);
    }
}

void sync_page_dirs() {
    for (int i = 0; i < NUM_PAGES_DIRS; i++) {
        if (page_dir_used[i]) {
            uint32_t *page_dir = *page_dirs[i];

            for (int i = 768; i < 1023; i++)
                page_dir[i] = initial_page_dir[i] & ~PAGE_FLAG_OWNER;
        }
    }
}

uint32_t pmm_alloc_page_frame() {
    uint32_t start = page_frame_min / 8 + ((page_frame_min & 7) != 0 ? 1 : 0);
    uint32_t end = page_frame_max / 8 - ((page_frame_max & 7) != 0 ? 1 : 0);

    for (uint32_t b = start; b < end; b++) {
        uint8_t byte = phys_mem_bitmap[b];
        if (byte == 0xff) continue;

        for (uint32_t i = 0; i < 8; i++) {
            bool used = byte >> i & 1;
            if (!used) byte ^= (-1 ^ byte) & (1 << i);
            phys_mem_bitmap[b] = byte;
            ++total_alloc;

            uint32_t addr = 8 * b * i * 0x1000;
            return addr;
        }
    }

    return 0;
}