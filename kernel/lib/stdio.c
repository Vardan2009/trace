#include "lib/stdio.h"

char *vmem = (char *)0xc00b8000;
int curx = 0, cury = 0;
unsigned char fg = COLOR_LGRAY;
unsigned char bg = COLOR_BLACK;
unsigned char color = 0x07;

void increment(void) {
    curx++;
    if (curx >= VGA_WIDTH) {
        curx = 0;
        shift_down();
    }
}

void decrement(void) {
    curx--;
    if (curx < 0) curx = 0;
    vmem[cury * VGA_WIDTH * 2 + curx * 2 + 1] = 0x07;
}

void shift_down(void) {
    cury++;
    if (cury >= VGA_HEIGHT) {
        cury = VGA_HEIGHT - 1;
        for (int y = 1; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH * 2; x++) {
                vmem[(y - 1) * VGA_WIDTH * 2 + x] = vmem[y * VGA_WIDTH * 2 + x];
            }
        }
        for (int x = 0; x < VGA_WIDTH * 2; x += 2) {
            vmem[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + x] = ' ';
            vmem[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + x + 1] = 0x07;
        }
    }
}

void putb() {
    decrement();
    putc(' ');
    decrement();
    set_cur_pos(cury * VGA_WIDTH + curx);
}

void putc(char c) {
    if (c == '\n') {
        curx = 0;
        shift_down();
    } else if (c == '\b')
        putb();
    else {
        vmem[cury * VGA_WIDTH * 2 + curx * 2] = c;
        vmem[cury * VGA_WIDTH * 2 + curx * 2 + 1] = color;
        increment();
    }
    set_cur_pos(cury * VGA_WIDTH + curx);
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
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vmem[i] = ' ';
        vmem[i + 1] = 0x07;
    }
    curx = 0;
    cury = 0;
}

void set_color(unsigned char _fg, unsigned char _bg) {
    fg = _fg;
    bg = _bg;
    color = (bg << 4) | (fg & 0x0F);
}

void set_color_fg(unsigned char _fg) {
    fg = _fg;
    color = (bg << 4) | (fg & 0x0F);
}

void set_color_bg(unsigned char _bg) {
    bg = _bg;
    color = (bg << 4) | (fg & 0x0F);
}

#include <stdarg.h>

void putc(char c); // Assume putc() is implemented
void puts(const char *s); // Assume puts() is implemented

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

void printui(unsigned int value, int width, char padChar) {
    char buffer[20];
    int i = 0;

    do {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    while (i < width) buffer[i++] = padChar; // Apply padding

    while (i > 0) putc(buffer[--i]);
}

void printhx(unsigned int value, int width, char padChar) {
    char buffer[20];
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value > 0) {
            int digit = value % 16;
            buffer[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
            value /= 16;
        }
    }

    while (i < width) buffer[i++] = padChar; // Apply padding

    while (i > 0) putc(buffer[--i]);
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;

            // Parse flags and width
            char padChar = ' ';
            if (*p == '0') { // Check for zero-padding
                padChar = '0';
                p++;
            }

            int width = 0;
            while (*p >= '0' && *p <= '9') { // Read width
                width = width * 10 + (*p - '0');
                p++;
            }

            // Handle specifier
            switch (*p) {
                case 's': {
                    char *str = va_arg(args, char *);
                    puts(str);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    printi(num); // No padding for signed integers
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    printui(num, width, padChar);
                    break;
                }
                case 'c': {
                    int num = va_arg(args, int);
                    putc((char)num);
                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    printhx(num, width, padChar);
                    break;
                }
                default:
                    putc('%');
                    putc(*p);
                    break;
            }
        } else {
            putc(*p);
        }
    }

    va_end(args);
}


void scanl(char *buffer, unsigned int size) {
    int i = 0;

    while (true) {
        char c = kb_readc();
        if (c == '\b') {
            if (i > 0) {
                putb();
                i--;
            }
            continue;
        }
        if (c == '\0') continue;
        if (c == '\n' || i >= size - 1) {
            putc('\n');
            break;
        }
        buffer[i++] = c;
        putc(c);
    }
    buffer[i] = '\0';
}

int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    scanl(buffer, 256);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 's': {
                    char *str = va_arg(args, char *);
                    int j = 0;
                    while (buffer[j] && buffer[j] != ' ' &&
                           j < sizeof(buffer) - 1) {
                        str[j] = buffer[j];
                        j++;
                    }
                    str[j] = '\0';
                    break;
                }
                case 'd': {
                    int *num = va_arg(args, int *);
                    *num = 0;
                    int sign = 1;
                    if (buffer[0] == '-') { sign = -1; }
                    for (int j = (sign == -1) ? 1 : 0;
                         buffer[j] >= '0' && buffer[j] <= '9'; j++) {
                        *num = *num * 10 + (buffer[j] - '0');
                    }
                    *num *= sign;
                    break;
                }
                case 'u': {
                    unsigned int *num = va_arg(args, unsigned int *);
                    *num = 0;
                    for (int j = 0; buffer[j] >= '0' && buffer[j] <= '9'; j++) {
                        *num = *num * 10 + (buffer[j] - '0');
                    }
                    break;
                }
                case 'c': {
                    char *ch = va_arg(args, char *);
                    *ch = buffer[0];
                    break;
                }
                default: break;
            }
        }
    }

    va_end(args);
    return 1;
}

void set_cur_pos(uint16_t position) {
    outb(VGA_CTRL_REG, 0x0F);
    outb(VGA_DATA_REG, (uint8_t)(position & 0xFF));
    outb(VGA_CTRL_REG, 0x0E);
    outb(VGA_DATA_REG, (uint8_t)((position >> 8) & 0xFF));
}