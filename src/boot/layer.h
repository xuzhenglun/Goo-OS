#ifndef LAYER_H_H
#define LAYER_H_H

#include "memory.h"

#define MAX_LAYERS 256
#define LAYER_USE  1

struct LAYER{
    unsigned char *buf;
    int bxsize,bysize,vx0,vy0,col_inv,height,flags;
    struct LAYER_CTL *ctl;
};

struct LAYER_CTL {
    unsigned char *vram,*map;
    int xsize,ysize,top;
    struct LAYER * layers_p[MAX_LAYERS];
    struct LAYER   layers[MAX_LAYERS];
};

struct LAYER_CTL * layer_ctl_init(struct MEMMAN *man, unsigned char *vram, int xsize ,int ysize);

struct LAYER * layer_alloc(struct LAYER_CTL *ctl);

void layer_setbuf(struct LAYER *lay, unsigned char *buf, int xsize, int ysize, int col_inv);

void layer_updown(struct LAYER * lay, int height);

void layer_refresh(struct LAYER * lay, int bx0, int by0, int bx1, int by1);

void layer_slide(struct LAYER * lay, int vx0, int vy0);

void layer_free(struct LAYER *lay);

void layer_refresh_sub(struct LAYER_CTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);

#endif
