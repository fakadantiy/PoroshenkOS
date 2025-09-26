#include "libs/keyboard.h"
#include "libs/vga.h"
#include "libs/terminal.h"

__attribute__((externally_visible)) _Noreturn void faka(void) { // GNU C only
    clearscreen();
    printstr("FUCKING POROSHENKOS 2.0!!!");
    terminal_run();
}
