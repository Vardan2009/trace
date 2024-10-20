#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdbool.h>
#include <stdint.h>

#include "lib/math.h"
#include "mem.h"

void init_kmalloc(uint32_t);
void change_heap_size(int);

#endif  // KMALLOC_H