#include "lib/malloc.h"

static uint8_t memory_pool[MEMORY_POOL_SIZE];
static Block* free_list = NULL;

void init_malloc() {
    free_list = (Block*)memory_pool;
    free_list->size = MEMORY_POOL_SIZE - sizeof(Block);
    free_list->free = true;
    free_list->next = NULL;
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    size_t total_size = size + sizeof(Block);
    Block* prev = NULL;
    Block* current = free_list;

    while (current) {
        if (current->free && current->size >= total_size) {
            // Split if there is enough space left
            if (current->size >= total_size + sizeof(Block)) {
                Block* new_block = (Block*)((uint8_t*)current + total_size);
                new_block->size = current->size - total_size;
                new_block->free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->free = false;
            return (void*)((uint8_t*)current + sizeof(Block));
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void free(void* ptr) {
    if (!ptr) return;

    Block* block = (Block*)((uint8_t*)ptr - sizeof(Block));
    block->free = true;

    Block* current = free_list;
    while (current) {
        if (current->free && current->next && current->next->free) {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
