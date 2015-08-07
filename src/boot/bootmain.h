#ifndef BOOTMAIN_H_H
#define BOOTMAIN_H_H

#include <sys/types.h>

#define ADR_BOOTINFO	0x00000ff0

void hlt(void);
void cli(void);
void io_out8(int16_t port,int8_t date);
int  io_load_eflags(void);
void io_store_eflags(int eflags);

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    char *vram;
};

#endif