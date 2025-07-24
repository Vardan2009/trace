#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdbool.h>
#include <stdint.h>

#include "lib/math.h"
#include "mem.h"

typedef struct kmalloc_header {
    uint32_t size;  // Size of block (without header)
    struct kmalloc_header *next;
    bool is_free;
} kmalloc_header_t;

void init_kmalloc(uint32_t);
void change_heap_size(uint32_t);

void *kmalloc(uint32_t);
void kfree(void *);

#endif  // KMALLOC_H