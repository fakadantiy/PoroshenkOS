#include "vga.h"

unsigned int cursor;

void printstr(const char *str) {
    unsigned int j = 0;
    while (str[j] != '\0') {
        VIDMEM[cursor] = str[j];
        VIDMEM[cursor+1] = 0x07;
        ++j;
        cursor += 2;
    }
}

void putchar(char c) {
    VIDMEM[cursor] = c;
    VIDMEM[cursor+1] = 0x07;
    cursor += 2;
}

void clearscreen(void) {
    unsigned int j = 0;
    while (j < 80*25*2) {
        VIDMEM[j] = ' ';
        VIDMEM[j+1] = 0x07;
        j += 2;
    }
}

