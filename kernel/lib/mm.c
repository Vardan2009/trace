#include "lib/mm.h"

static uint8_t memory_pool[MEMORY_POOL_SIZE];
static Block* memory_blocks;
static size_t total_blocks;

void malloc_init() {
    total_blocks = MEMORY_POOL_SIZE / sizeof(Block);
    memory_blocks = (Block*)memory_pool;

    for (size_t i = 0; i < total_blocks; i++) {
        memory_blocks[i].size = sizeof(Block);
        memory_blocks[i].free = true;
    }
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    for (size_t i = 0; i < total_blocks; i++) {
        if (memory_blocks[i].free && memory_blocks[i].size >= size) {
            memory_blocks[i].free = false;
            if (memory_blocks[i].size > size + sizeof(Block)) {
                Block* new_block =
                    (Block*)((uint8_t*)memory_blocks + (i * sizeof(Block)) +
                             sizeof(Block) + size);
                new_block->size = memory_blocks[i].size - sizeof(Block) - size;
                new_block->free = true;
                memory_blocks[i].size = size;
            }
            return (void*)((uint8_t*)memory_blocks + (i * sizeof(Block)) +
                           sizeof(Block));
        }
    }

    return NULL;
}

void free(void* ptr) {
    if (!ptr) return;

    uintptr_t address = (uintptr_t)ptr;
    if (address < (uintptr_t)memory_pool ||
        address >= (uintptr_t)(memory_pool + MEMORY_POOL_SIZE))
        return;

    Block* block = (Block*)((uint8_t*)ptr - sizeof(Block));
    block->free = true;

    for (size_t i = 0; i < total_blocks; i++) {
        if (memory_blocks[i].free) {
            if (i < total_blocks - 1 && memory_blocks[i + 1].free) {
                memory_blocks[i].size +=
                    memory_blocks[i + 1].size + sizeof(Block);
                for (size_t j = i + 1; j < total_blocks - 1; j++)
                    memory_blocks[j] = memory_blocks[j + 1];
                total_blocks--;
            }
        }
    }
}