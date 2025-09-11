char *VIDMEM = (char*)0xB8000; // видеопамять
#define KEYBOARD_PORT 0x60 // клавиатура
unsigned char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',   0,'\\',
    'z','x','c','v','b','n','m',',','.','/',   0,   '*',  0, ' '
};

unsigned int cursor;
unsigned char last_scancode = 0;

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}


void printstr(const char *str) {
    unsigned int j = 0;

    while (str[j] != '\0') { // писать буковки пока не закончатся
        VIDMEM[cursor] = str[j];
        VIDMEM[cursor+1] = 0x07; // цвет 
        ++j;
        cursor += 2;
    }
}

void putchar(char c) {
	VIDMEM[cursor] = c; //вписывать буковку там где курсор
	VIDMEM[cursor+1] = 0x07; // цвет
	cursor += 2;
}

void clearscreen() {
    unsigned int j = 0;
    while (j < 80 * 25 * 2) {
        VIDMEM[j] = ' ';    // заполняем все пробелами чтобы нихуя не было
        VIDMEM[j+1] = 0x07; 
        j += 2;
    }
}

void keyboard_handler(void) {
    unsigned char scancode = inb(KEYBOARD_PORT); // читаем порт клавиатуры

    if (scancode != last_scancode) { // реагируем только на новое событие
        last_scancode = scancode;    // ← обязательно обновляем!

        if (scancode & 0x80) {      // клавиша отпущена
            return;
        }

        char symbol = scancode_to_ascii[scancode]; // преобразуем
        if (symbol) {
            if (symbol == '\b') {   // backspace
                if (cursor > 0) {
                    cursor -= 2;
                    VIDMEM[cursor] = ' ';
                    VIDMEM[cursor+1] = 0x07;
                    cursor -= 2;
                }
            } else if (symbol == '\n') { // enter
                cursor = (cursor / (80*2) + 1) * (80*2);
            } else {                     // обычный символ
                putchar(symbol);
            }
        }
    }
}

void faka(void) {
    clearscreen();
    printstr("FUCKING POROSHENKOS 2.0!!!");
    while(1) {
    	keyboard_handler();
    }
}
