#pragma once

#define VIDMEM ((char*)0xB8000)

extern unsigned int cursor;

void printstr(const char *str);
void putchar(char c);
void clearscreen(void);
void newline();
