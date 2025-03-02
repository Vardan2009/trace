#include "lib/stdlib.h"

char toupper(char c) {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}