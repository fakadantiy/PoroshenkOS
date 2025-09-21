#include "vga.h"

unsigned int cursor;

void printstr(const char *str) {
    newline();
    unsigned int j = 0;
    while (str[j] != '\0') {
        VIDMEM[cursor] = str[j];
	VIDMEM[cursor+1] = 0x07;
        ++j;
        cursor += 2;
    }
}

void printclr(const char *str, unsigned char clr) {
    newline();
    unsigned int j = 0;
    while (str[j] != '\0') {
        VIDMEM[cursor] = str[j];
        VIDMEM[cursor+1] = clr;
        ++j;
        cursor += 2;
    }
}

void newline() {
    cursor = (cursor / (80*2) + 1) * (80*2);
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

