#ifndef BOOTMAIN_H_H
#define BOOTMAIN_H_H

#define ADR_BOOTINFO	0x00000ff0
#define CON_X           512
#define CON_Y           330
//#define CON_X           800
//#define CON_Y           600
#define CON_TEXT_X      CON_X - 16
#define CON_TEXT_Y      CON_Y - 37

#include "layer.h"

void hlt(void);
void cli(void);
void io_out8(short port,char date);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
void task_cons_main(struct LAYER *lay_win_b);
int cons_newline(int y, struct LAYER *layer);

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    char *vram;
};

#endif
