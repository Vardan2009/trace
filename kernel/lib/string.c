#include "lib/string.h"

void *memset(void *s, int c, size_t n) {
    unsigned char *ptr = s;
    while (n--) *ptr++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) {
            *(--d) = *(--s);
        }
    }
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcpy(char *dest, const char *src) {
    char *ptr = dest;
    while ((*ptr++ = *src++)) { }
    return dest;
}

size_t strlen(const char *s) {
    const char *ptr = s;
    while (*ptr) ptr++;
    return ptr - s;
}

char *strcat(char *dest, const char *src) {
    char *ptr = dest;
    while (*ptr) {
        ptr++;
    }
    while ((*ptr++ = *src++)) { }
    return dest;
}