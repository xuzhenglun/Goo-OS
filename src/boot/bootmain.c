#include "bootmain.h"
#include "graph.h"
#include "dsctbl.h"
#include "int.h"
#include "../golibc/stdio.h"
#include "basic.h"

void bootmain(void) {
    init_palette();
    init_gdtidt();
    init_pic();
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

    init_keyboard();
    enable_mouse();

    char keybuf[32],mousebuf[1024];
    fifo8_init(&keyfifo,   32,  keybuf  );
    fifo8_init(&mousefifo, 1024, mousebuf);

    sti();
    io_out8(PIC0_IMR, 0xf9); /* PIC1とキーボードを許可(11111001) */
    io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */

    int kflag,mflag,mouse_phase;
    unsigned char mouse_dbuff[3];

    for(;;){
        kflag = fifo8_status(&keyfifo);
        mflag = fifo8_status(&mousefifo);
        if( kflag || mflag ){
            cli();
            if(kflag)
            {
              unsigned char i = fifo8_get(&keyfifo);
              sti();
              char s[4];
              sprintf(s ,"%02X", i );
              boxfill8(binfo->vram, binfo->scrnx, COL8_BLACK, 0, 25, 15, 40);
              print_fonts(binfo->vram, binfo->scrnx, 0, 25, COL8_WHITE, s );
             }
            cli();
            if(mflag)
            {
                unsigned char i = fifo8_get(&mousefifo);
                sti();
                switch(mouse_phase){
                    case 0:
                        {
                            if(i == 0xfa) mouse_phase = 1;
                            break;
                        }
                    case 1:
                        {
                            mouse_dbuff[0] = i;
                            mouse_phase  = 2;
                            break;
                        }
                    case 2:
                        {
                            mouse_dbuff[1] = i;
                            mouse_phase   = 3;
                            break;
                        }
                    case 3:
                        {
                            mouse_dbuff[2] = i;
                            mouse_phase   = 1;\
                            break;
                        }
                }
                char s[10];
                sprintf(s ,"%02X %02X %02X", mouse_dbuff[0], mouse_dbuff[1], mouse_dbuff[2]);
                boxfill8(binfo->vram, binfo->scrnx, COL8_BLACK, 32, 25, 32+8*8-1, 40);
                print_fonts(binfo->vram, binfo->scrnx, 32, 25, COL8_WHITE, s );
            }
        }
        else
        stihlt();
    }
}





