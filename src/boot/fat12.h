#ifndef FAT32_H_H
#define FAT32_H_H

#define ADR_DISKIMG 0x8000

struct FILENAME {
    unsigned char name[8],ext[3];
};

struct FILEINFO {
    struct FILENAME filename;
    unsigned char type;
    char reserve[10];
    unsigned short time,date,clustno;
    unsigned int size;
};

void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);

#endif
