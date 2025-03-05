#include "lib/stdio.h"
#include "lib/string.h"
#include "lib/malloc.h"

#define HEAP_SIZE (1024 * 1024)
#define BLOCK_SIZE 16
#define NUM_BLOCKS (HEAP_SIZE / BLOCK_SIZE)
#define SET_BIT(b, i) ((b) |= (1 << (i)))
#define CLEAR_BIT(b, i) ((b) &= ~(1 << (i)))
#define CHECK_BIT(b, i) ((b) & (1 << (i)))
typedef struct { int blocks; } header_t;

static char heap[HEAP_SIZE];
static unsigned char bitmap[NUM_BLOCKS / 8];

static int find_free_blocks(int num_blocks) {
    int free_count = 0, start_index = -1;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        int bi = i / 8, bj = i % 8;
        if (!CHECK_BIT(bitmap[bi], bj)) {
            if (free_count == 0) start_index = i;
            free_count++;
            if (free_count == num_blocks) return start_index;
        } else {
            free_count = 0;
        }
    }
    return -1;
}

void *malloc(size_t size) {
    if (!size || size + sizeof(header_t) > HEAP_SIZE) return 0;
    size_t total = sizeof(header_t) + size;
    int num_blocks = (total + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int start = find_free_blocks(num_blocks);
    if (start == -1) return 0;
    for (int i = start; i < start + num_blocks; i++) {
        int bi = i / 8, bj = i % 8;
        SET_BIT(bitmap[bi], bj);
    }
    header_t *h = (header_t *)(heap + start * BLOCK_SIZE);
    h->blocks = num_blocks;
    return (void *)((char *)h + sizeof(header_t));
}

void free(void *ptr) {
    if (!ptr || ptr < (void *)heap || ptr >= (void *)(heap + HEAP_SIZE)) return;
    header_t *h = (header_t *)((char *)ptr - sizeof(header_t));
    int start = ((char *)h - heap) / BLOCK_SIZE;
    int num_blocks = h->blocks;
    for (int i = start; i < start + num_blocks; i++) {
        int bi = i / 8, bj = i % 8;
        CLEAR_BIT(bitmap[bi], bj);
    }
}

size_t allocated_size(void *ptr) {
    if (!ptr) return 0;
    header_t *h = (header_t *)((char *)ptr - sizeof(header_t));
    return h->blocks * BLOCK_SIZE - sizeof(header_t);
}

void *realloc(void *ptr, size_t new_size) {
    if (!ptr) return malloc(new_size);
    if (!new_size) { free(ptr); return 0; }
    size_t old_size = allocated_size(ptr);
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return 0;
    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    free(ptr);
    return new_ptr;
}

void print_bitmap() {
    for (int i = 0; i < NUM_BLOCKS / 8; i++) {
        for (int j = 0; j < 8; j++)
            printf("%d", CHECK_BIT(bitmap[i], j) ? 1 : 0);
        printf(" ");
    }
    printf("\n");
}