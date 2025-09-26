#pragma once

#define VIDMEM ((char*)0xB8000)

extern unsigned short cursor;

void printstr(const char *str);
void putchar(char c);
void clearscreen(void);
void newline();
void update_hw_cursor();
