#include "lib/malloc.h"

static uint8_t memory_pool[MEMORY_POOL_SIZE];
static Block* memory_blocks;
static size_t total_blocks;

void init_malloc() {
    total_blocks = 1;
    memory_blocks = (Block*)memory_pool;
    memory_blocks[0].size = MEMORY_POOL_SIZE / total_blocks;
    memory_blocks[0].free = true;
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    // Adjust size to include header.
    size += sizeof(Block);

    // Search for a free block.
    for (size_t i = 0; i < total_blocks; i++) {
        if (memory_blocks[i].free && memory_blocks[i].size >= size) {
            memory_blocks[i].free = false;

            // Split if there's enough space.
            if (memory_blocks[i].size >= size + sizeof(Block)) {
                Block* new_block = (Block*)((uint8_t*)memory_blocks + (i * sizeof(Block)) + size);
                new_block->size = memory_blocks[i].size - size;
                new_block->free = true;
                memory_blocks[i].size = size;
            }

            return (void*)((uint8_t*)&memory_blocks[i] + sizeof(Block)); // Correct pointer calculation
        }
    }

    return NULL;  // No suitable block found.
}

void free(void* ptr) {
    if (!ptr) return;

    uintptr_t address = (uintptr_t)ptr;
    if (address < (uintptr_t)memory_pool || address >= (uintptr_t)(memory_pool + MEMORY_POOL_SIZE)) return;

    Block* block = (Block*)((uint8_t*)ptr - sizeof(Block));
    block->free = true;

    // Merge adjacent free blocks.
    for (size_t i = 0; i < total_blocks - 1; i++) {
        if (memory_blocks[i].free && memory_blocks[i + 1].free) {
            memory_blocks[i].size += memory_blocks[i + 1].size + sizeof(Block);

            // Shift the blocks after i + 1 to the left
            for (size_t j = i + 1; j < total_blocks - 1; j++) {
                memory_blocks[j] = memory_blocks[j + 1];
            }
            total_blocks--;  // After merge, reduce the block count
            i--;  // Move back one step to check the merged block
        }
    }
}
