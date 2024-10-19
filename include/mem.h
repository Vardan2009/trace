#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "lib/stdio.h"
#include "multiboot.h"

void init_memory(multiboot_info*);

#endif  // MEM_H