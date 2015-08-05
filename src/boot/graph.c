#ifndef GRAPH_H_H
#define GRAPH_H_H
#endif
#include "graph.h"

#ifndef BASIC_H_H
#define BASIC_H_H
#endif
#include "basic.h"

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
        0x00,0x00,0x00,    //ºÚ
        0xff,0x00,0x00,    //ÁÁºì
        0x00,0xff,0x00,    //ÁÁÂÌ
        0xff,0xff,0x00,    //ÁÁ»Æ
        0x00,0x00,0xff,    //ÁÁÀ¶
        0xff,0x00,0xff,    //ÁÁ×Ï
        0x00,0xff,0xff,    //Ç³ÁÁÀ¶
        0xff,0xff,0xff,    //°×
        0xc6,0xc6,0xc6,    //ÁÁ»Ò
        0x84,0x00,0x00,    //°µºì
        0x00,0x84,0x00,    //°µÂÌ
        0x84,0x84,0x00,    //°µ»Æ
        0x00,0x00,0x84,    //°µÇà
        0x84,0x00,0x84,    //°µ×Ï
        0x00,0x84,0x84,    //Ç³°µÀ¶
        0x84,0x84,0x84     //°µ»Ò
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

void init_screen8(char * vram, int xsize,int ysize){
	boxfill8(vram ,xsize,COL8_LD_BLUE     ,0          ,0          ,xsize -1  ,ysize - 29 );
    boxfill8(vram ,xsize,COL8_GREY        ,0          ,ysize - 28 ,xsize -1  ,ysize - 28 );
    boxfill8(vram ,xsize,COL8_WHITE       ,0          ,ysize - 27 ,xsize -1  ,ysize - 27 );
    boxfill8(vram ,xsize,COL8_GREY        ,0          ,ysize - 26 ,xsize -1  ,ysize - 1  );

    boxfill8(vram ,xsize,COL8_WHITE       ,3          ,ysize - 24 ,59        ,ysize - 24 );
    boxfill8(vram ,xsize,COL8_WHITE       ,2          ,ysize - 24 ,2         ,ysize - 4  );
    boxfill8(vram ,xsize,COL8_D_GREY      ,3          ,ysize - 4  ,59        ,ysize - 4  );
    boxfill8(vram ,xsize,COL8_D_GREY      ,59         ,ysize - 23 ,59        ,ysize - 5  );
    boxfill8(vram ,xsize,COL8_BLACK       ,2          ,ysize - 3  ,59        ,ysize - 3  );
    boxfill8(vram ,xsize,COL8_BLACK       ,60         ,ysize - 24 ,60        ,ysize - 3  );

    boxfill8(vram ,xsize,COL8_D_GREY      ,xsize - 47 ,ysize - 24 ,xsize - 4 ,ysize - 24 );
    boxfill8(vram ,xsize,COL8_D_GREY      ,xsize - 47 ,ysize - 23 ,xsize - 47,ysize - 4  );
    boxfill8(vram ,xsize,COL8_WHITE       ,xsize - 47 ,ysize - 3  ,xsize - 3 ,ysize - 3  );
    boxfill8(vram ,xsize,COL8_WHITE       ,xsize - 3  ,ysize - 24 ,xsize - 3 ,ysize - 3  );

}