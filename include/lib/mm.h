#ifndef MM_H
#define MM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_POOL_SIZE 1024 * 1024  // 1 MB memory pool

typedef struct Block {
    size_t size;
    bool free;
} Block;

static uint8_t memory_pool[MEMORY_POOL_SIZE];
static bool memory_allocated[MEMORY_POOL_SIZE];

void malloc_init();
void *malloc(size_t);
void free(void *);

#endif  // MM_H