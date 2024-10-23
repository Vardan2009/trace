#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);
char *strcat(char *dest, const char *src);
int memcmp(const void *ptr1, const void *ptr2, size_t num);

#endif  // STRING_H
