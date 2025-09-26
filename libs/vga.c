#include "vga.h"

#include "RoshenLibC/sys/io.h"

unsigned short cursor;

void update_hw_cursor() {
	// this only works on VGA but is faster than int10h
	outb(0x3D4, 0x0F); // address for low byte of cursor
	outb(0x3D5, cursor/2 & 0xFF); // write it
	outb(0x3D4, 0x0E); // high byte of cursor
	outb(0x3D5, (cursor/2 >> 8) & 0xFF); // write it
	// that's a shit ton of divisions, this shit's expensive on the CPU
	// unless on Via
}


void printstr(const char *str) {
    unsigned int j = 0;
    while (str[j] != '\0') {
        VIDMEM[cursor] = str[j];
        VIDMEM[cursor+1] = 0x07;
        ++j;
        cursor += 2;
    }
    update_hw_cursor();
}

void newline() {
	cursor = (cursor / (80*2) + 1) * (80*2);
	update_hw_cursor();
}

void putchar(char c) {
    VIDMEM[cursor] = c;
    VIDMEM[cursor+1] = 0x07;
    cursor += 2;
    update_hw_cursor();
}

void clearscreen(void) {
    unsigned int j = 0;
    while (j < 80*25*2) {
        VIDMEM[j] = ' ';
        VIDMEM[j+1] = 0x07;
        j += 2;
    }
}

