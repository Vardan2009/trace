#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlen(const char *s);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
char *strchr(const char *str, int c);
const char *strrchr(const char *str, int ch);
char *strtok(char *str, const char *delim);
char *strdup(const char *str);

#endif  // STRING_H
