#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "lib/math.h"
#include "lib/stdio.h"
#include "multiboot.h"

#define KERNEL_START 0xc0000000
#define KERNEL_MALLOC 0xd0000000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
#define PAGE_FLAG_OWNER (1 << 9)
#define REC_PAGEDIR ((uint32_t *)0xfffff000)
#define REC_PAGETABLE(i) ((uint32_t *)(0xffc00000 + ((i) << 12)))

#define PAGE_SIZE 0x1000

extern uint32_t initial_page_dir[1024];

uint32_t pmm_alloc_page_frame();
void sync_page_dirs();
uint32_t *mem_get_current_page_dir();
void mem_change_page_dir(uint32_t *);
void mem_map_page(uint32_t, uint32_t, uint32_t);

void invalidate(uint32_t);

void init_memory(uint32_t, uint32_t);

#endif  // MEM_H