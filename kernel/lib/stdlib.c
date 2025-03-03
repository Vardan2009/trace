#include "lib/stdlib.h"

char toupper(char c) {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}

int isdigit(char c) {
    return (c >= '0') && (c <= '9');
}

void itoa(int value, char *buffer) {
    char temp[12];
    int i = 0, is_negative = 0;

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    do {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    if (is_negative) temp[i++] = '-';

    int a = 0;
    for (int j = i - 1; j >= 0; --j)
        buffer[a++] = temp[j];
    buffer[a] = '\0';
}