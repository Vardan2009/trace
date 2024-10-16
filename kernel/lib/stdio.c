#include "lib/stdio.h"

char *vmem = (char *)0xb8000;
int curx = 0, cury = 0;
unsigned char color = 0x07;

void increment(void) {
    curx++;
    if (curx >= WW) {
        curx = 0;
        shift_down();
    }
}

void shift_down(void) {
    cury++;
    if (cury >= WH) {
        cury = WH - 1;
        for (int y = 1; y < WH; y++) {
            for (int x = 0; x < WW * 2; x++) {
                vmem[(y - 1) * WW * 2 + x] = vmem[y * WW * 2 + x];
            }
        }
        for (int x = 0; x < WW * 2; x += 2) {
            vmem[(WH - 1) * WW * 2 + x] = ' ';
            vmem[(WH - 1) * WW * 2 + x + 1] = 0x07;
        }
    }
}

void putc(char c) {
    if (c == '\n') {
        curx = 0;
        shift_down();
    } else {
        vmem[cury * WW * 2 + curx * 2] = c;
        vmem[cury * WW * 2 + curx * 2 + 1] = color;
        increment();
    }
}

void puts(const char *message) {
    int i = 0;
    while (message[i]) {
        if (message[i] == '\n') {
            curx = 0;
            shift_down();
            i++;
            continue;
        }
        putc(message[i++]);
    }
}

void clear_screen() {
    for (int i = 0; i < WW * WH * 2; i += 2) {
        vmem[i] = ' ';
        vmem[i + 1] = 0x07;
    }
    curx = 0;
    cury = 0;
}

void set_color(unsigned char fg, unsigned char bg) {
    color = (bg << 4) | (fg & 0x0F);
}

void printi(int value) {
    if (value < 0) {
        putc('-');
        value = -value;
    }

    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i > 0) putc(buffer[--i]);
}

void printui(unsigned int value) {
    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i > 0) putc(buffer[--i]);
}

void printhx(unsigned int value) {
    char buffer[20];
    int i = 0;

    if (value == 0) {
        putc('0');
        return;
    }

    while (value > 0) {
        int digit = value % 16;
        buffer[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        value /= 16;
    }

    while (i > 0) putc(buffer[--i]);
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's': {
                    char *str = va_arg(args, char *);
                    puts(str);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    printi(num);
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    printui(num);
                    break;
                }
                case 'c': {
                    int num = va_arg(args, int);
                    putc((char)num);
                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    printhx(num);
                    break;
                }
                default:
                    putc('%');
                    putc(*p);
                    break;
            }
        } else
            putc(*p);
    }

    va_end(args);
}

char getc() {
    return scank().chr;
}