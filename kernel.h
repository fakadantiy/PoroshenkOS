#pragma once
extern volatile unsigned long clock;
typedef unsigned long uptr;
struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));
struct idt_entry idt[256] __attribute__((aligned(16)));
