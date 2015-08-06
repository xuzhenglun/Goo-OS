#ifndef GRAPH_H_H
#define GRAPH_H_H

#define COL8_BLACK    0
#define COL8_RED      1
#define COL8_GREEN    2
#define COL8_YELLOW   3
#define COL8_BLUE     4
#define COL8_PURPLR   5
#define COL8_L_BLUE   6
#define COL8_WHITE    7
#define COL8_GREY     8
#define COL8_D_RED    9
#define COL8_D_GREEN  10
#define COL8_D_YELLOW 11
#define COL8_D_BLUE   12
#define COL8_D_PURPLE 13
#define COL8_LD_BLUE  14
#define COL8_D_GREY   15


void init_palette(void);
void set_palette(int start,int end,unsigned char *rbg);
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0,int x1, int y1);
void putfont8(char *vram, int xsize,int x,int y,char c,char *font);
void print_fonts(char *vram, int xsize, int x, int y, char c, char *s);
void init_mouse_cursor(char * mouse, char bc);
void putblock8_8(char *vram,int vxsize, int pxsize,int pysize,int px0,int py0, char *buf,int bxsize);
void init_screen8(char * vram, int xsize,int ysize);
void make_window8(char * vram, int xsize,int ysize,char *title);

#endif