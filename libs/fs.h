#ifndef FS_H
#define FS_H

#define MAX_FILES 128
#define MAX_FILENAME 32
#define MAX_CONTENT 512

typedef struct {
    char name[MAX_FILENAME];
    char content[MAX_CONTENT];
    int used;
} File;

typedef struct {
    File files[MAX_FILES];
} FileSystem;

extern FileSystem fs;

int fs_create(const char* name);
int fs_delete(const char* name);
int fs_write(const char* name, const char* data);
const char* fs_read(const char* name);
void fs_ls(void);

#endif
