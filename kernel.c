char *videomem = (char*)0xB8000;

void printstr(const char *str) {
    unsigned int i = 0, j = 0;

    while (str[j] != '\0') {
        videomem[i] = str[j];
        videomem[i+1] = 0x07; 
        ++j;
        i += 2;
    }
}

void clearscreen() {
    unsigned int j = 0;
    while (j < 80 * 25 * 2) {
        videomem[j] = ' ';    
        videomem[j+1] = 0x07; 
        j += 2;
    }
}

void faka(void) {
    clearscreen();
    printstr("FUCKING POROSHENKOS 2.0!!!");
}
