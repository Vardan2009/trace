#include "lib/string.h"

#include "lib/malloc.h"

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
        while (n--) { *(--d) = *(--s); }
    }
    return dest;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return n == 0 ? 0 : *(unsigned char *)s1 - *(unsigned char *)s2;
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
    while ((*ptr++ = *src++)) {}
    return dest;
}

const char *strrchr(const char *str, int ch) {
    const char *last = NULL;
    while (*str) {
        if (*str == (char)ch) last = str;
        str++;
    }
    return last;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ptr = dest;
    while (n > 0 && *src) {
        *ptr++ = *src++;
        --n;
    }
    while (n > 0) {
        *ptr++ = '\0';
        --n;
    }
    return dest;
}

size_t strlen(const char *s) {
    const char *ptr = s;
    while (*ptr) ptr++;
    return ptr - s;
}

char *strcat(char *dest, const char *src) {
    char *ptr = dest;
    while (*ptr) { ptr++; }
    while ((*ptr++ = *src++)) {}
    return dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *ptr = dest;
    while (*ptr) { ptr++; }
    while (n-- && (*ptr++ = *src++)) {}
    *ptr = '\0';
    return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
    const unsigned char *p1 = (const unsigned char *)ptr1;
    const unsigned char *p2 = (const unsigned char *)ptr2;

    for (size_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) { return (p1[i] < p2[i]) ? -1 : 1; }
    }

    return 0;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == c) return (char *)str;
        str++;
    }
    if (c == '\0') return (char *)str;
    return NULL;
}

char *strtok(char *str, const char *delim) {
    static char *next_token = NULL;  // Static pointer to store the next token
    if (str != NULL) {
        next_token = str;  // Start from the beginning of the new string
    }

    if (next_token == NULL) {
        return NULL;  // No more tokens
    }

    // Skip leading delimiters
    char *start = next_token;
    while (*start && strchr(delim, *start)) { start++; }

    if (*start == '\0') {
        next_token = NULL;  // No tokens left
        return NULL;
    }

    // Find the end of the token
    char *end = start;
    while (*end && !strchr(delim, *end)) { end++; }

    if (*end == '\0') {
        next_token = NULL;  // No more tokens after this one
    } else {
        *end = '\0';           // Null-terminate the token
        next_token = end + 1;  // Move the pointer to the next token
    }

    return start;
}

char *strdup(const char *str) {
    if (str == NULL) {
        return NULL;  // Return NULL if the input string is NULL
    }

    size_t len = strlen(str) + 1;      // +1 for the null terminator
    char *copy = (char *)malloc(len);  // Allocate memory for the copy

    if (copy == NULL) {
        return NULL;  // Return NULL if memory allocation fails
    }

    strcpy(copy, str);  // Copy the original string into the new memory location
    return copy;
}


long strtol(const char *str, char **endptr, int base) {
    long result = 0;
    int sign = 1;
    
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    if (base == 0) {
        if (*str == '0') {
            str++;
            if (*str == 'x' || *str == 'X') {
                base = 16;  // hexadecimal
                str++;
            } else {
                base = 8;  // octal
            }
        } else {
            base = 10;  // decimal
        }
    }
    
    while ((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F')) {
        int digit;
        if (*str >= '0' && *str <= '9') {
            digit = *str - '0';
        } else if (*str >= 'a' && *str <= 'f') {
            digit = *str - 'a' + 10;
        } else {
            digit = *str - 'A' + 10;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        str++;
    }
    
    if (endptr != NULL)
        *endptr = (char *)str;

    return result * sign;
}
