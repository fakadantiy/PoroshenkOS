#include "kernel.h"
#include "libs/keyboard.h"
#include "libs/vga.h"
#include "libs/terminal.h"
#include "libs/RoshenLibC/sys/io.h"

typedef unsigned long uptr;
// uptr is a special name that makes the compiler shut the fuck up about casting

struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));
// interrupt dibil table
struct idt_entry idt[256] __attribute__((aligned(16)));

void set_idt_gate(unsigned char n, void (* const handler)()) {
	uptr addr = (uptr)handler;
	idt[n].offset_low  = addr & 0xFFFF;
	idt[n].selector    = 0x08; // bs that makes no sence
	idt[n].zero        = 0; // sure ig
	idt[n].type_attr   = 0x8E; // why not
	idt[n].offset_high = (addr >> 16) & 0xFFFF;
}
struct idt_ptr {
    unsigned short limit;
    unsigned base;
} __attribute__((packed));

_Noreturn void kernel_panica(const char *err) {
	asm ("cli"); // if we are already in an interrupt, don't interrupt our interruption
	//cursor = 0;
	set_text_mode();
	printstr("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA YOUR POROSHENKO ENCOUNTERED A PROBLEM ASHIPKA STOP ");
	printstr(err);
	printstr(" TAK ZHE PONYATNO KAK EYO RESCHIT'");
	while (1) {asm ("hlt");} // pizdec' here
}

void isr_0() {kernel_panica("DIVISION BY ZERO");}
void isr_1() {kernel_panica("DEBUG");}
void isr_2() {kernel_panica("\"PIZDEC' COMPYUTERU\"");}
// unimplemented
// unimplemented
void isr_5() {kernel_panica("BOUND");}
void isr_6() {kernel_panica("INVALID OPCODE");}
void isr_7() {kernel_panica("OPCODE NOT AVAILABLE");}
void isr_8() {kernel_panica("a, nu tut ya chto-to ne tak sdelal");}
// reserved
// can't happen in this OS
void isr_11() {kernel_panica("NO SEGMENT");}
void isr_12() {kernel_panica("STACK");}
void isr_13() {kernel_panica("PROTECT");}
void isr_14() {kernel_panica("RAM");}
// reserved
void isr_16() {kernel_panica("MATH");}
//void isr_16() {printstr("math error");} //debug
void isr_17() {kernel_panica("ALIGN");}
void isr_18() {kernel_panica("CRITICAL PIZDEC' TO YOUR PC");}
// can't happen with these compile settings
// can't happen in this OS
void isr_21() {kernel_panica("COMPILER NIGERIAN");}

__attribute__((externally_visible)) volatile unsigned long clock = 0;

struct interrupt_frame {unsigned eip, cs, eflags; }; // makes gcc stfu

__attribute__((interrupt)) __attribute__((target("arch=i386"))) __attribute__((externally_visible))
void isr_112_c(struct interrupt_frame* frame) {
	io_wait();outb(0x70, 0x0C);io_wait();inb(0x71);
	clock++; 
	io_wait();outb(0xA0, 0x20);io_wait();outb(0x20, 0x20);
	// please see comment in terminal.c
} // we selling clocks now bitch
__attribute__((interrupt)) __attribute__((target("arch=i386"))) __attribute__((externally_visible))
void isr_clear_irq(struct interrupt_frame* frame) {
	io_wait();outb(0xA0, 0x20);io_wait();outb(0x20, 0x20);
	// this can skip real IRQs but who tf cares
}

__attribute__((interrupt)) __attribute__((target("arch=i386"))) __attribute__((externally_visible)) void nothing(struct interrupt_frame* frame) {} //debug
__attribute__((naked, externally_visible))
void int_stub(void) {
    asm volatile("iret");
}

__attribute__((interrupt)) __attribute__((target("arch=i386"))) __attribute__((externally_visible))
void irq2_handler(struct interrupt_frame* frame) {
    outb(0x20, 0x20); // EOI master only (not slave!)
}


// from osdev wiki
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

// shit's required in protected mode apparently or smth
void PIC_remap(unsigned short const offset1, unsigned short const offset2) {
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	
	
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	
	outb(PIC2_DATA, ICW4_8086);
	

	// Unmask both PICs.
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}
void IRQ_set_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;

    if(IRQline < 8) {
        port = 0x21;
    } else {
        port = 0xa1;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

void IRQ_clear_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;

    if(IRQline < 8) {
        port = 0x21;
    } else {
        port = 0xa1;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

// from chatgptigga
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned base;
} __attribute__((packed));

struct gdt_entry gdt[3]; // null, code, data
struct gdt_ptr   gdtp;

void gdt_set_entry(const unsigned short num, const unsigned base, const unsigned limit, const unsigned char access, const unsigned char gran) {
    gdt[num].base_low    = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = limit & 0xFFFF;
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_install() {
    gdt_set_entry(0, 0, 0, 0, 0);            // Null descriptor
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF); // Code: base=0, limit=4 GB, exec/read, ring0
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF); // Data: base=0, limit=4 GB, read/write, ring0

    gdtp.limit = sizeof(gdt) - 1;
    gdtp.base  = (uptr)&gdt;

    // asm: load GDT and jump to flush CS
    asm (
        "lgdt (%0)\n"
        "jmp $0x08, $.1\n" // far jump to flush CS
        ".1:\n"
        "mov $0x10, %%ax\n" // load data segments
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        : "r" (&gdtp)
        : "memory", "ax"
    );
}

__attribute__((externally_visible)) _Noreturn void faka(void) { // GNU C only
	gdt_install();
	// this shitass function is needed on grub/limine but not via direct seabios boot
	// this is because whatever the fuck those bootloaders do with the gdt
	// makes everything work except when any irq is recieved
	// in which case the very first instruction results in testicular torsion
	// spent 2 more days debugging this, imagine my face in gdb
	
	set_text_mode();
	
	struct idt_ptr idtp;
	idtp.limit = sizeof(idt) - 1;
	idtp.base  = (uptr)&idt;

	for (int n = 0; n < 256; ++n) {
	    uptr addr = (uptr)nothing;
	    //uptr addr = (uptr)int_stub;
	    // same thing written differently, just debugging
	    idt[n].offset_low  = addr & 0xFFFF;
	    idt[n].selector    = 0x08; // bs that makes no sence
	    idt[n].zero        = 0; // sure ig
	    idt[n].type_attr   = 0x8E; // why not
	    idt[n].offset_high = (addr >> 16) & 0xFFFF;
	}

	// mask all IRQs
	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);
	
	// add everything
	set_idt_gate(0, isr_0);
	set_idt_gate(1, isr_1);
	set_idt_gate(2, isr_2);
	set_idt_gate(5, isr_5);
	set_idt_gate(6, isr_6);
	set_idt_gate(7, isr_7);
//	set_idt_gate(8, isr_8); // i fucked up somewhere, see terminal.c comment
	set_idt_gate(11, isr_11);
	set_idt_gate(12, isr_12);
	set_idt_gate(13, isr_13);
	set_idt_gate(14, isr_14);
	set_idt_gate(16, isr_16);
	set_idt_gate(17, isr_17);
	set_idt_gate(18, isr_18);
	set_idt_gate(21, isr_21);
	//set_idt_gate(112, isr_112_asm); // this is the default, right?
	
	// conflict of types with set_idt_gate, setting manually

	// irq 8 (clock)
	idt[112].offset_low  = (uptr)isr_112_c & 0xFFFF;
	idt[112].selector    = 0x08; // bs that makes no sence
	idt[112].zero        = 0; // sure ig
	idt[112].type_attr   = 0x8E; // why not
	idt[112].offset_high = ((uptr)isr_112_c >> 16) & 0xFFFF;	
	
	//PIC_remap(0x08, 0x70);

	// clear spurious irq

	idt[0x0f].offset_low  = (uptr)isr_clear_irq & 0xFFFF;
	idt[0x0f].selector    = 0x08; // bs that makes no sence
	idt[0x0f].zero        = 0; // sure ig
	idt[0x0f].type_attr   = 0x8E; // why not
	idt[0x0f].offset_high = ((uptr)isr_clear_irq >> 16) & 0xFFFF;	
	idt[0x77].offset_low  = (uptr)isr_clear_irq & 0xFFFF;
	idt[0x77].selector    = 0x08; // bs that makes no sence
	idt[0x77].zero        = 0; // sure ig
	idt[0x77].type_attr   = 0x8E; // why not
	idt[0x77].offset_high = ((uptr)isr_clear_irq >> 16) & 0xFFFF;	

	// irq 2
	idt[0x0a].offset_low  = (uptr)irq2_handler & 0xFFFF;
	idt[0x0a].selector    = 0x08; // bs that makes no sence
	idt[0x0a].zero        = 0; // sure ig
	idt[0x0a].type_attr   = 0x8E; // why not
	idt[0x0a].offset_high = ((uptr)irq2_handler >> 16) & 0xFFFF;	


	// you gotta tell it where it is
	asm("lidt %0" : : "m"(idtp));
	asm("sti"); // let them ALL inasm("sti"); // let them ALL in

	IRQ_clear_mask(2); // for good luck
	IRQ_clear_mask(8); 
	
	// osdev wiki enable my clock bitch
	// cuz we selling clocks now bitch
	
	outb(0x70, 0x8B);		// select register B, and disable NMI
	io_wait();
	char prev=inb(0x71);	// read the current value of register B
	io_wait();
	outb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
	io_wait();
	outb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	io_wait();
	outb(0x70, 0x8A);		// set index to register A, disable NMI
	io_wait();
	prev=inb(0x71);	// get initial value of register A
	io_wait();
	outb(0x70, 0x8A);		// reset index to A
	io_wait();
	outb(0x71, (prev & 0xF0) | 3); //write only our rate to A. Note, rate is the bottom 4 bits.
	
    printstr("FUCKING POROSHENKOS 2.0!!!");

    terminal_run();
    //play_video();
}

