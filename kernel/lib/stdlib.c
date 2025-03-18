#include "lib/stdlib.h"

#include <stddef.h>

#define INT_MAX 2147483647
#define INT_MIN (-2147483647 - 1)

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
    for (int j = i - 1; j >= 0; --j) buffer[a++] = temp[j];
    buffer[a] = '\0';
}

int stoi(const char *str) {
    if (str == NULL) return 0;

    int i = 0, sign = 1;
    long result = 0;

    while (str[i] == ' ') i++;
    if (str[i] == '-' || str[i] == '+') {
        if (str[i] == '-') sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        if (result * sign > INT_MAX) return INT_MAX;
        if (result * sign < INT_MIN) return INT_MIN;
        i++;
    }
    return (int)(result * sign);
}

double stof(const char *str) {
    double result = 0.0;
    double factor = 1.0;

    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;

    if (*str == '-') {
        factor = -1.0;
        str++;
    } else if (*str == '+')
        str++;

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    if (*str == '.') {
        str++;
        double decimal_place = 1.0;
        while (*str >= '0' && *str <= '9') {
            decimal_place *= 0.1;
            result += (*str - '0') * decimal_place;
            str++;
        }
    }

    return result * factor;
}

float fmod(float x, float y) {
    if (y == 0.0f) return 0.0f;

    float quotient = x / y;
    float integer_part;
    float fractional_part = modff(quotient, &integer_part);

    return x - (integer_part * y);
}

float modff(float value, float *iptr) {
    if (value == 0.0f) {
        *iptr = 0.0f;
        return 0.0f;
    }
    *iptr = (float)((int)value);
    return value - *iptr;
}
