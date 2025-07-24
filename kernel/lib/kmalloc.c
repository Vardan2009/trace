#include "lib/kmalloc.h"

static uint32_t heap_start;
static uint32_t heap_size;
static uint32_t threshold;
static bool kmalloc_initialized = false;

#define ALIGN4(x) (((x) + 3) & ~3)
#define HEADER_SIZE (sizeof(kmalloc_header_t))
#define MIN_SPLIT_SIZE 16

static kmalloc_header_t *heap_head = NULL;

void change_heap_size(uint32_t new_sz) {
    int oldPageTop = CEILDIV(heap_size, 0x1000);
    int newPageTop = CEILDIV(new_sz, 0x1000);

    if (newPageTop > oldPageTop) {
        int diff = newPageTop - oldPageTop;

        for (int i = 0; i < diff; i++) {
            uint32_t phys = pmm_alloc_page_frame();
            mem_map_page(KERNEL_MALLOC + oldPageTop * 0x1000 + i * 0x1000, phys,
                         PAGE_FLAG_WRITE);
        }
    }

    heap_size = new_sz;
}

kmalloc_header_t *find_free_block(uint32_t size) {
    kmalloc_header_t *curr = heap_head;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            if (curr->size >= size + HEADER_SIZE + MIN_SPLIT_SIZE) {
                kmalloc_header_t *split_block =
                    (kmalloc_header_t *)((uint8_t *)(curr + 1) + size);
                split_block->size = curr->size - size - HEADER_SIZE;
                split_block->is_free = true;
                split_block->next = curr->next;

                curr->size = size;
                curr->next = split_block;
            }

            curr->is_free = false;
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

kmalloc_header_t *request_space(uint32_t size) {
    uint32_t total_size = ALIGN4(size + HEADER_SIZE);

    uint32_t new_end = heap_start + heap_size + total_size;
    if (new_end > heap_start + threshold)
        change_heap_size(new_end - heap_start);

    kmalloc_header_t *block = (kmalloc_header_t *)(heap_start + heap_size);
    block->size = size;
    block->is_free = false;
    block->next = NULL;

    if (!heap_head) {
        heap_head = block;
    } else {
        kmalloc_header_t *last = heap_head;
        while (last->next) last = last->next;
        last->next = block;
    }

    heap_size += total_size;
    return block;
}

void *kmalloc(uint32_t size) {
    if (!kmalloc_initialized) return NULL;

    size = ALIGN4(size);

    kmalloc_header_t *block = find_free_block(size);
    if (!block) {
        block = request_space(size);
        printf("requested space\n");
        if (!block) return NULL;
    }

    printf("kmalloced %x\n", size);

    return (void *)(block + 1);
}

void coalesce_free_blocks() {
    kmalloc_header_t *curr = heap_head;

    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void kfree(void *ptr) {
    if (!ptr) return;

    kmalloc_header_t *block = ((kmalloc_header_t *)ptr) - 1;
    block->is_free = true;
    printf("kfreed %x\n", block->size);

    coalesce_free_blocks();
}

void init_kmalloc(uint32_t initial_heap_sz) {
    heap_start = KERNEL_MALLOC;
    heap_size = 0;
    threshold = 0;
    heap_head = NULL;
    kmalloc_initialized = true;

    change_heap_size(initial_heap_sz);
}
