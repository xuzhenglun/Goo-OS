#ifndef FAT32_H_H
#define FAT32_H_H

#define ADR_DISKIMG 0x8000

struct FILEINFO {
    unsigned char name[8],ext[3],type;
    char reserve[10];
    unsigned short time,date,clustno;
    unsigned int size;
};

#endif
