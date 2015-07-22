#include "libc.h"
#include "../golibc/stdio.h"

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

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx,scrny;
    char *vram;
};

void init_palette(void);
void set_palette(int start,int end,unsigned char *rbg);
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0,int x1, int y1);
void putfont8(char *vram, int xsize,int x,int y,char c,char *font);
void print_fonts(char *vram, int xsize, int x, int y, char c, char *s);
void init_mouse_cursor(char * mouse, char bc);
void putblock8_8(char *vram,int vxsize, int pxsize,int pysize,int px0,int py0, char *buf,int bxsize);



void bootmain(void) {
    init_palette();
    /*-------------------INIT-PALETTE-COMPLETED-------------------------*/

    char * vram;
    int xsize;
    int ysize;
    struct BOOTINFO *binfo;

    binfo = (struct BOOTINFO *) 0x0ff0;
    xsize = binfo->scrnx;
    ysize = binfo->scrny;
    vram  = binfo->vram;

    boxfill8(vram,xsize,COL8_LD_BLUE     ,0          ,0          ,xsize -1  ,ysize - 29 );
    boxfill8(vram,xsize,COL8_GREY        ,0          ,ysize - 28 ,xsize -1  ,ysize - 28 );
    boxfill8(vram,xsize,COL8_WHITE       ,0          ,ysize - 27 ,xsize -1  ,ysize - 27 );
    boxfill8(vram,xsize,COL8_GREY        ,0          ,ysize - 26 ,xsize -1  ,ysize - 1  );

    boxfill8(vram,xsize,COL8_WHITE       ,3          ,ysize - 24 ,59        ,ysize - 24 );
    boxfill8(vram,xsize,COL8_WHITE       ,2          ,ysize - 24 ,2         ,ysize - 4  );
    boxfill8(vram,xsize,COL8_D_GREY      ,3          ,ysize - 4  ,59        ,ysize - 4  );
    boxfill8(vram,xsize,COL8_D_GREY      ,59         ,ysize - 23 ,59        ,ysize - 5  );
    boxfill8(vram,xsize,COL8_BLACK       ,2          ,ysize - 3  ,59        ,ysize - 3  );
    boxfill8(vram,xsize,COL8_BLACK       ,60         ,ysize - 24 ,60        ,ysize - 3  );

    boxfill8(vram,xsize,COL8_D_GREY      ,xsize - 47 ,ysize - 24 ,xsize - 4 ,ysize - 24 );
    boxfill8(vram,xsize,COL8_D_GREY      ,xsize - 47 ,ysize - 23 ,xsize - 47,ysize - 4  );
    boxfill8(vram,xsize,COL8_WHITE       ,xsize - 47 ,ysize - 3  ,xsize - 3 ,ysize - 3  );
    boxfill8(vram,xsize,COL8_WHITE       ,xsize - 3  ,ysize - 24 ,xsize - 3 ,ysize - 3  );

    char s[40];
    sprintf(s,"%d * %d ",xsize,ysize);
    print_fonts(vram,xsize,8,8,COL8_WHITE,s);

    char mcursor[16*16];
    int mx,my;
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor(mcursor,COL8_LD_BLUE);
    putblock8_8(vram,xsize,16,16,mx,my,mcursor,16);



    /*--------------------------HLT-------------------------------------*/
    for(;;){
         hlt();
    }
}

void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0,int x1, int y1){
    int x,y;
    for(y=y0;y<=y1;y++){
         for(x=x0;x<=x1;x++)
             vram[y*xsize+x]=c;
    }
    return;
}

void init_palette(void){
    static unsigned char table_rgb[16*3] = {
        0x00,0x00,0x00,    //黑
        0xff,0x00,0x00,    //亮红
        0x00,0xff,0x00,    //亮绿
        0xff,0xff,0x00,    //亮黄
        0x00,0x00,0xff,    //亮蓝
        0xff,0x00,0xff,    //亮紫
        0x00,0xff,0xff,    //浅亮蓝
        0xff,0xff,0xff,    //白
        0xc6,0xc6,0xc6,    //亮灰
        0x84,0x00,0x00,    //暗红
        0x00,0x84,0x00,    //暗绿
        0x84,0x84,0x00,    //暗黄
        0x00,0x00,0x84,    //暗青
        0x84,0x00,0x84,    //暗紫
        0x00,0x84,0x84,    //浅暗蓝
        0x84,0x84,0x84     //暗灰
    };
    set_palette(0,15,table_rgb);
    return;
}

void set_palette(int start,int end,unsigned char *rgb){
     int eflags;
     eflags=io_load_eflags();
     cli();
     io_out8(0x03c8,start);
     for(int i = start; i<= end; i++){
         io_out8(0x03c9,rgb[0] /4);
         io_out8(0x03c9,rgb[1] /4);
         io_out8(0x03c9,rgb[2] /4);
         rgb +=3;
     }
     io_store_eflags(eflags);
     return;
}


void putfont8(char *vram, int xsize,int x,int y,char c,char *font){
    char d;
    for(int i= 0;i < 16 ; i++){
        d=font[i];
        if((d & 0x80 ) != 0 ) { vram[( y + i ) * xsize + x + 0] = c; }
        if((d & 0x40 ) != 0 ) { vram[( y + i ) * xsize + x + 1] = c; }
        if((d & 0x20 ) != 0 ) { vram[( y + i ) * xsize + x + 2] = c; }
        if((d & 0x10 ) != 0 ) { vram[( y + i ) * xsize + x + 3] = c; }
        if((d & 0x08 ) != 0 ) { vram[( y + i ) * xsize + x + 4] = c; }
        if((d & 0x04 ) != 0 ) { vram[( y + i ) * xsize + x + 5] = c; }
        if((d & 0x02 ) != 0 ) { vram[( y + i ) * xsize + x + 6] = c; }
        if((d & 0x01 ) != 0 ) { vram[( y + i ) * xsize + x + 7] = c; }
    }
}


void print_fonts(char *vram, int xsize, int x, int y, char c, char *s){
    extern char  _binary_font_bin_start[4096];
     for(; *s != '\0' ;s++){
         putfont8(vram,xsize,x,y,c,_binary_font_bin_start + *s *16);
         x += 8;
     }
}

void init_mouse_cursor(char * mouse, char bc){
    static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
    };

    for(int y =0 ; y< 16 ; y++){
        for( int x =0 ; x <16 ;x++ ){
            if (cursor[y][x] == '*'){
                mouse[16*y+x] = COL8_BLACK;
            }
            if (cursor[y][x] == 'O'){
                mouse[16*y+x] = COL8_WHITE;
            }
            if (cursor[y][x] == '.'){
                mouse[16*y+x] = bc;
            }
        }
    }
}


void putblock8_8(char *vram,int vxsize, int pxsize,int pysize,int px0,int py0, char *buf,int bxsize){
    for(int y = 0;y < pysize; y++){
        for(int x = 0;x < pxsize; x++){
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
            }
    }
}
