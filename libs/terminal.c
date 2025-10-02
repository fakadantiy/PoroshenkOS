#include "keyboard.h"
#include "vga.h"
#include "string.h"
#include "fs.h"
#include "ata.h"

char buf[128];
int len = 0;

void terminal_run(void) {
    printstr("\n> ");
    char c;
    while (1) {
        c = keyboard_getchar();
        if (!c) continue;
    
    switch (c) {
        case '\n' :
            buf[len] = 0;

            char *cmd = buf;
            char *arg = 0;
            for (int i = 0; i < len; i++) {
                if (buf[i] == ' ') {
                    buf[i] = 0;      // конец строки для команды
                    arg = &buf[i+1]; // аргумент после пробела
                    break;
                }
            }

       if (strcmp(cmd, "ls") == 0) {
        fs_ls();
       } else if (strcmp(cmd, "mk") == 0) {
        if (arg) {
            fs_create(arg); 
            printstr("\nCreated file\n");
        } else printclr("No filename \n", 0x4);
       } else if (strcmp(cmd, "mkdir") == 0) {
	if (arg) {
	    fs_mkdir(arg);
	    printstr("\nCreated directory\n");
	}
       } else if (strcmp(cmd, "del") == 0) {
        if (arg) { 
            fs_delete(arg); 
            printstr("\nDeleted file\n");
        } else printclr("No filename \n", 0x4);
	} else if (strcmp(cmd, "deldir") == 0) {
	 if (arg) {
	    fs_rmdir(arg);
	    printstr("\nDeleted directory\n");
	 } else printclr("\nNo directory name \n", 0x4);
	} else if (strcmp(cmd, "cd") == 0) {
	 if (arg) {
	    fs_cd(arg);
	 } else printclr("\nNo directory name\n", 0x4);
            } else {
                printclr("\nUnknown command \n", 0x4);
            }
            len = 0;
            printstr("> ");
       break;
    case '\b':
       if (len > 0) {
        len--;
        cursor -= 2;
        putchar(' ', 0x07);
        cursor -= 2;
       }
       break;
    default:
            if (len < 127) buf[len++] = c;
            putchar(c, 0x07);
    }
    }    
} 
