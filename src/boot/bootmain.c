#include "libc.h"

void init_palette(void);
void set_palette(int start,int end,unsigned char *rbg);

void bootmain(void) {
    char *p;
    int i;
    init_palette();
    p=(char *) 0xa0000;
    for(i=0;i<=0xffff;i++){
        p[i] = i & 0x0f;
    }
    for(;;){
         hlt();
    }
}

void init_palette(void){
    static unsigned char table_rgb[16*3] = {
        0x00,0x00,0x00,
        0xff,0x00,0x00,
        0x00,0xff,0x00,
        0xff,0xff,0x00,
        0x00,0x00,0xff,
        0xff,0x00,0xff,
        0x00,0xff,0xff,
        0xff,0xff,0xff,
        0xc6,0xc6,0xc6,
        0x84,0x00,0x00,
        0x00,0x84,0x00,
        0x84,0x84,0x00,
        0x00,0x00,0x84,
        0x84,0x00,0x84,
        0x00,0x84,0x84,
        0x84,0x84,0x84
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
         io_out8(0x03c9,rgb[3] /4);
         io_out8(0x03c9,rgb[4] /4);
         io_out8(0x03c9,rgb[5] /4);
         rgb +=3;
     }
     io_store_eflags(eflags);
     return;
}
