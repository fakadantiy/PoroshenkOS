#include "fs.h"
#include "string.h"
#include "vga.h"

FileSystem fs;

int fs_create(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.files[i].used) {
            fs.files[i].used = 1;
            strcpy(fs.files[i].name, name);
            fs.files[i].content[0] = 0;
            return 1;
        }
    }
    return 0;
}

int fs_delete(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {
            fs.files[i].used = 0;
            return 1;
        }
    }
    return 0;
}

int fs_write(const char* name, const char* data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {
            strcpy(fs.files[i].content, data);
            return 1;
        }
    }
    return 0;
}

const char* fs_read(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used && strcmp(fs.files[i].name, name) == 0) {
            return fs.files[i].content;
        }
    }
    return 0;
}

void fs_ls(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].used) {
            printstr(fs.files[i].name);
        }
    }
}
