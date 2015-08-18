#include "graph.h"

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

void make_window8(char * buf, int xsize,int ysize,char *title, int act){
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};

    char tc,tbc;
    if(act != 0){
        tc  = COL8_BLACK;
        tbc = COL8_D_BLUE;
    }else{
        tc  = COL8_GREY;
        tbc = COL8_D_GREY;
    }

    boxfill8(buf, xsize, COL8_GREY, 0,         0,         xsize - 1, 0        );
    boxfill8(buf, xsize, COL8_WHITE, 1,         1,         xsize - 2, 1        );
    boxfill8(buf, xsize, COL8_GREY, 0,         0,         0,         ysize - 1);
    boxfill8(buf, xsize, COL8_WHITE, 1,         1,         1,         ysize - 2);
    boxfill8(buf, xsize, COL8_D_GREY, xsize - 2, 1,         xsize - 2, ysize - 2);
    boxfill8(buf, xsize, COL8_BLACK, xsize - 1, 0,         xsize - 1, ysize - 1);
    boxfill8(buf, xsize, COL8_GREY, 2,         2,         xsize - 3, ysize - 3);
    boxfill8(buf, xsize, tbc      , 3,         3,         xsize - 4, 20       );
    boxfill8(buf, xsize, COL8_D_GREY, 1,         ysize - 2, xsize - 2, ysize - 2);
    boxfill8(buf, xsize, COL8_BLACK, 0,         ysize - 1, xsize - 1, ysize - 1);
    print_fonts(buf, xsize, 24, 4, tc, title);
	for (int y = 0; y < 14; y++) {
		for (int x = 0; x < 16; x++) {
			char c = closebtn[y][x];
			if (c == '@') {
                c = COL8_BLACK;
			} else if (c == '$') {
                c = COL8_D_GREY;
			} else if (c == 'Q') {
                c = COL8_GREY;
			} else {
                c = COL8_WHITE;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void print_refreshable_font(struct LAYER *lay,int x,int y, int color, int background, char * s){
    int len = 0;
    for(char * l = s; *l != '\0'; l++ ){
        len++;
    }
    boxfill8((char *)lay->buf, lay->bxsize, background, x, y, x + len * 8,y + 16);
    print_fonts((char *)lay->buf,lay->bxsize,x,y,color,s);
    layer_refresh(lay,x,y,x + len * 8, y + 16);
}

void make_textbox(struct LAYER * lay, int x0, int y0, int sx, int sy, int c){
    int x1 = x0 + sx;
    int y1 = y0 + sy;
    boxfill8(lay->buf, lay->bxsize, COL8_D_GREY, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
    boxfill8(lay->buf, lay->bxsize, COL8_D_GREY, x0 - 3, y0 - 3, x0 - 1, y1 + 1);
    boxfill8(lay->buf, lay->bxsize, COL8_WHITE , x0 - 3, y1 + 2, x1 + 1, y1 + 2);
    boxfill8(lay->buf, lay->bxsize, COL8_WHITE , x1 + 2, y0 - 3, x1 + 2, y1 + 2);
    boxfill8(lay->buf, lay->bxsize, COL8_BLACK , x0 - 1, y0 - 2, x1 + 0, y0 - 2);
    boxfill8(lay->buf, lay->bxsize, COL8_BLACK , x0 - 2, y0 - 2, x0 - 2, y1 + 0);
    boxfill8(lay->buf, lay->bxsize, COL8_GREY  , x0 - 2, y1 + 1, x1 + 0, y1 + 1);
    boxfill8(lay->buf, lay->bxsize, COL8_GREY  , x1 + 1, y0 - 2, x1 + 1, y1 + 1);
    boxfill8(lay->buf, lay->bxsize, c          , x0 - 1, y0 - 1, x1 + 0, y1 + 0);
}
