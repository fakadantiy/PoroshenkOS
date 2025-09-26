#include "keyboard.h"
#include "vga.h"
#include "string.h"

char buf[128]; // в квадратных скобках если че размер буффера
int len = 0;

_Noreturn void terminal_run(void) {
    newline();
    printstr("> ");
    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            buf[len] = 0;

            if (strcmp(buf, "help") == 0) {
		newline();
                printstr("help - show this");
	    } else if (strcmp(buf, "putin") == 0) {
		newline();
		printstr("O VELIKIY PUTIN+POROSHENKO");
            } else {
		newline();
                printstr("unknown");
            }
	    newline();
            len = 0;
            printstr("> ");
        } else if (c == '\b') {
	    if (len > 0) {
		len--;
		cursor -= 2;
		putchar(' ');
		cursor -= 2;
	    }
	} else {
            if (len < 127) buf[len++] = c;
            putchar(c);
        }
    }
}
