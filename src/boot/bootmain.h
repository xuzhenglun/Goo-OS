#ifndef BOOTMAIN_H_H
#define BOOTMAIN_H_H

#define ADR_BOOTINFO	0x00000ff0
#define CON_X           512
#define CON_Y           330
#define CON_TEXT_X      CON_X - 16
#define CON_TEXT_Y      CON_Y - 37

#include "layer.h"

void bootmain(void);
void task_cons_main(struct LAYER *lay_win_b);

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    unsigned char *vram;
};

#endif
