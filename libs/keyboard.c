#include "keyboard.h"
#include "vga.h"

unsigned char last_scancode = 0;

unsigned char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '
};

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void keyboard_handler(void) {
    unsigned char scancode = inb(KEYBOARD_PORT);

    if (scancode != last_scancode) {
        last_scancode = scancode;

        if (scancode & 0x80) return; // клавиша отпущена

        char symbol = scancode_to_ascii[scancode];
        if (symbol) {
            if (symbol == '\b') {
                // backspace
                if (cursor > 0) {
                    cursor -= 2;
                    VIDMEM[cursor] = ' ';
                    VIDMEM[cursor+1] = 0x07;
                    cursor -= 2;
                }
            } else if (symbol == '\n') {
                // enter
                cursor = (cursor / (80*2) + 1) * (80*2);
            } else {
                putchar(symbol);
            }
        }
    }
}
