#ifndef VGA
#define VGA

#define VIDMEM ((char*)0xB8000)
#define VIDMEM_G ((char*)0xA0000)

extern bool isInGraphicsMode;
extern volatile unsigned short cursor;

void printstr(const char *str);
void putchar(char c);
void clearscreen(void);
void newline();
void update_hw_cursor();
void write_regs(unsigned char *regs);
void set_rgb332_palette(void);
void set_graphics_mode();
void set_text_mode();
void putpixel(unsigned char, short, short);
void drawchar(unsigned char, int, int);

#endif
