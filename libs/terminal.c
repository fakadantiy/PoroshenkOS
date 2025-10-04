#include "keyboard.h"
#include "vga.h"
#include "RoshenLibC/string.h"
#include "RoshenLibC/sys/io.h"
#include "commodore64.h"
#include "commodore64_part2.h"
#include "../kernel.h"

char buf[128]; // в квадратных скобках если че размер буффера
int tlen = 0;

_Noreturn void play_video() {
	static const char marquee[] = "New Project Redhat Systemd Virus Exploit GCC Cyberweapon Redhat Threat New Project Redhat Systemd Nanorobot Built into GCC Module for Redhat Linux GCC Exploit from Redhat on the Quantum-Physical-Mathematical Level Systemd USSR Unidentified Flying Object Redhat Microsoft Aliens Area 51 GCC Bender in Linux Antimatter CIA Intelligence Services Redhat Linux GCC Surveillance of People and Open-Source Bender of Alien Scale US Intelligence Systemd and Redhat's New Project to Seize Earth Systemd GCC Top-Secret Development Redhat Systemd Secret Jewish Civilization Commodore 64 ";
	// 1 space at the end to not leave behind garbage from previous characters
	unsigned frame = 0;
	unsigned long counter = clock, text_counter = clock, change_counter = clock, sound_counter = clock, sound_pos = 0;
	signed text_x = 320, char_x;
	unsigned char old_0x61_bits = inb(0x61);
	outb(0x61, inb(0x61) | 3);
	set_graphics_mode();
	while (frame <= commodore64_size/58880) { // 320x184
		memcpy(VIDMEM_G, commodore64+(frame++*58880), 58880);
		// the debug
		//while(counter+337>=clock){asm("sti\nhlt");}
		while (counter+340>=clock) { // ~24.x fps, technically 341.4 but too slow because poroshenkos
			io_wait();outb(0xA0, 0x20);io_wait();outb(0x20, 0x20);asm("sti");
			// ALL of this SHOULD already be done in the interrupt
			// but it ISN'T
			// I SPENT 2 DAYS DEBUGGING THIS
			// WHY
			if (change_counter%59 != clock%59) {
				change_counter = clock;
				text_x = 320-(clock-text_counter)/59;
				// 59 is the speed, found in a very scientific way of fucking around and finding out
				for (unsigned c = 0; c <= 575; c++) { // 575 = len(marquee)
					char_x = text_x+(c*9);
					if (char_x > -9 && char_x < 320) {
						drawchar(marquee[c], char_x, 184);
					}
				}
			}
			// we ain't ticking fast enough to do PWM for 8-bit audio, sooooooo
			// 1 FUCKING BIT AUDIO, FUCK YEAH
			if (clock-sound_counter != sound_pos) {
				sound_pos = clock-sound_counter;
				if (commodore64_audio[sound_pos] > 127) {
					io_wait();
					outb(0x61, old_0x61_bits | 0x01);
					//VIDMEM_G[0] = 0; //debug
				} else {
					io_wait();
					outb(0x61, old_0x61_bits & ~0x01);
					//VIDMEM_G[0] = 255; //debug
				} 
			}
		}
		counter=clock;
	}
	set_text_mode();
	// this does NOT work for some reason
	// but ONLY after playing this shitass video
	// sooooo...
	
	for (unsigned char c = 0; c < 255; c++) {
	    idt[c].offset_low  = 0;
	    idt[c].selector    = 0;
	    idt[c].zero        = 0;
	    idt[c].type_attr   = 0;
	    idt[c].offset_high = 0; // double
	}
	asm volatile("lidt %0" :: "m"(idt));
	asm("int $0x10;"); // single
	// TRIPLE FAULT BABEEEEEHHHHHHHHH

	// niger (country), why doesn't this work
	// try the almighty CF9
	outb(0xCF9, 0x06);
	// try keyboard controller
	outb(0x64, 0xFE);
	
	while (1) {asm("hlt");} // just to make compiler shut up (hopefully)
}

// who the fuck wrote ts
// what the actual fuck is the ident

_Noreturn void terminal_run(void) {
    newline();
    printstr("> ");
    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            buf[tlen] = 0;

            if (strcmp(buf, "help") == 0) {
		newline();
                printstr("help - show this");
	    } else if (strcmp(buf, "putin") == 0) {
		newline();
		printstr("O VELIKIY PUTIN+POROSHENKO");
		} else if (strcmp(buf, "commodore64") == 0) {play_video();
            } else {
		newline();
                printstr("unknown");
            }
	    newline();
            tlen = 0;
            printstr("> ");
        } else if (c == '\b') {
		    if (tlen > 0) {
				tlen--;
				cursor -= 2;
				putchar(' ');
				cursor -= 2;
				update_hw_cursor();
		    }
	} else {
            if (tlen < 127) buf[tlen++] = c;
            putchar(c);
        }
    }
}
