#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include "driver/keyboard.h"

#define WW 80
#define WH 25

#define COLOR_BLACK   0x0
#define COLOR_BLUE    0x1
#define COLOR_GREEN   0x2
#define COLOR_AQUA    0x3
#define COLOR_RED     0x4
#define COLOR_PURPLE  0x5
#define COLOR_YELLOW  0x6
#define COLOR_LGRAY   0x7
#define COLOR_DGRAY   0x8
#define COLOR_DGRAY   0x8
#define COLOR_LGREEN  0xA
#define COLOR_LAQUA   0xB
#define COLOR_LRED    0xC
#define COLOR_LPURPLE 0xD
#define COLOR_LYELLOW 0xE
#define COLOR_WHITE   0xF

extern char *vmem;
extern unsigned char color;
extern int curx, cury;

void putc(char);
void puts(const char *);

void printi(int);
void printui(unsigned int);
void printhx(unsigned int);
void printf(const char *, ...);

void clear_screen(void);

void increment();
void shift_down();

void set_color(unsigned char, unsigned char);

char getc();

#endif  // STDIO_H