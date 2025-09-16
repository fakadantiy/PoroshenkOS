#pragma once
#include "vga.h"

#define KEYBOARD_PORT 0x60
extern unsigned char last_scancode;

char keyboard_getchar(void);
