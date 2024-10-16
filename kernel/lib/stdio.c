#include "lib/stdio.h"

char *vmem = (char *)0xb8000;
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
    } else {
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
    color = (COLOR_BLACK << 4) | (fg & 0x0F);
}

void set_color_bg(unsigned char _bg) {
    bg = _bg;
    color = (COLOR_BLACK << 4) | (fg & 0x0F);
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

void scanl(char *buffer, unsigned int size) {
    int i = 0;

    while (true) {
        uint8_t k = get_scancode();
        if (k == 0x2a) {
            shifted = 1;
            continue;
        }
        if (k == 0xaa) {
            shifted = 0;
            continue;
        }
        if (k == 0x0e) {
            if (i > 0) {
                i--;
                putb();
            }
            continue;
        }
        key sk = getk(k);
        char c = shifted ? sk.shifted_char : sk.chr;
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