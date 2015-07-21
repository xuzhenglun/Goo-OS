#include "libc.h"

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
void puts(char *vram, int xsize, int x, int y, char c, char *s);

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


    /*extern char  _binary_font_bin_start[4096];*/
    /*static char font[16] = {*/
        /*0x00,0x18,0x18,0x18,0x18,0x24,0x24,0x24,*/
        /*0x24,0x7e,0x42,0x42,0x42,0xe7,0x00,0x00*/
    /*};*/
    puts(vram,xsize,8,8,COL8_WHITE,"HOLY SHIT");

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


void puts(char *vram, int xsize, int x, int y, char c, char *s){
    extern char  _binary_font_bin_start[4096];
     for(; *s != '\0' ;s++){
         putfont8(vram,xsize,x,y,c,_binary_font_bin_start + *s *16);
         x += 8;
     }
}
