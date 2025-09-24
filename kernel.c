#include "libs/keyboard.h"
#include "libs/vga.h"
#include "libs/terminal.h"

void faka(void) {
    clearscreen();
    disable_cursor();
    printstr("Hello, and welcome to the PoroshenkOS 1.1.3");
    terminal_run();
}
