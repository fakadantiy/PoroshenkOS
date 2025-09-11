#include "libs/keyboard.h"
#include "libs/vga.h"

void faka(void) {
    clearscreen();
    printstr("FUCKING POROSHENKOS 2.0!!!");
    while(1) {
    	keyboard_handler();
    }
}
