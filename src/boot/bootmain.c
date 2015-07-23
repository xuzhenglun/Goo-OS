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

    sti();
    io_out8(PIC0_IMR, 0xf9); /* PIC1とキーボードを許可(11111001) */
    io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
    /*--------------------------HLT-------------------------------------*/
    for(;;){
         hlt();
    }
}





