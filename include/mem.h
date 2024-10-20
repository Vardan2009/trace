#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "lib/math.h"
#include "lib/stdio.h"
#include "multiboot.h"

#define KERNEL_START 0xc0000000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)

extern uint32_t initial_page_dir[1024];

void invalidate(uint32_t);

void init_memory(uint32_t, uint32_t);

#endif  // MEM_H