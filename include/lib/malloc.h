#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

void *malloc(size_t);
void free(void *);
void *realloc(void *, size_t);
void print_bitmap();

#endif  // MALLOC_H