#ifndef MALLOC_H
#define MALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_POOL_SIZE (1024 * 1024)  // 1 MB memory pool

typedef struct Block {
    size_t size;
    bool free;
    struct Block* next;
} Block;

void init_malloc();
void* malloc(size_t);
void *realloc(void *, size_t);
void free(void *);

#endif  // MALLOC_H