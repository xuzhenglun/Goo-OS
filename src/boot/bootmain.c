#define MEMMAN_ADDR 0x00280000

#include "bootmain.h"
#include "graph.h"
#include "dsctbl.h"
#include "int.h"
#include "../golibc/stdio.h"
#include "basic.h"
#include "memory.h"
#include "layer.h"
#include "timer.h"


void bootmain(void) {
    struct LAYER_CTL * layctl;                     //初始化定义层控制体
    struct LAYER    *lay_back,*lay_mouse,*lay_win;        //定义鼠标层和背景层
    char *buf_back,*buf_mouse,*buf_win;
    init_palette();
    init_gdtidt();
    init_pic();
    init_pit();

    struct BOOTINFO *binfo;                        //从内存中找到在IPL中保存的图形参数
    binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40];                                    //文字输出缓存，（在栈中）

    char keybuf[32],mousebuf[128],timerbuf[8];                //鼠标和键盘中断缓存，（在栈中）
    fifo8_init(&keyfifo,   32,  keybuf  );
    fifo8_init(&mousefifo, 128, mousebuf);
    fifo8_init(&timerfifo, 8,   timerbuf);

    settimer(1000, &timerfifo, 1);

    unsigned int memtotal;                      //内存初始化
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR; //内存管理块的内存位置
    memtotal = memtest(0x00300000, 0xbfffffff); //总内存大小
    mem_init(memman);                            //初始化内存控制块
    mem_free(memman, 0x00300000,memtotal - 0x00300000); //初始化内存页


    layctl = layer_ctl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);  //初始化层控制块
    lay_back = layer_alloc(layctl);                                            //创建背景层
    lay_mouse = layer_alloc(layctl);                                        //创建鼠标层
    lay_win   = layer_alloc(layctl);                                        //创建窗口层
    buf_back = (unsigned char *) mem_alloc_4k(memman,binfo->scrnx * binfo->scrny);
    buf_mouse = (unsigned char *) mem_alloc_4k(memman,16*16);
    buf_win   = (unsigned char *) mem_alloc_4k(memman,160 * 52);
    layer_setbuf(lay_back,buf_back,binfo->scrnx,binfo->scrny,-1);             //设定背景层
    layer_setbuf(lay_mouse,buf_mouse,16,16,99);                                //设定鼠标层（透明色为99）
    layer_setbuf(lay_win  ,buf_win,160,52,-1);


    init_screen8(buf_back, binfo->scrnx,binfo->scrny);                         //画一下桌面
    make_window8(buf_win,160,52,"COUNTER");

    sprintf(s,"| MEMORY:%dMB|FREE:%dKB",memtotal /(1024*1024),mem_total(memman)/1024); //输出内存消息
    print_fonts(buf_back ,binfo->scrnx,100,8,COL8_WHITE,s);

    sprintf(s,"%d * %d ",binfo->scrnx,binfo->scrny);                                    //输出屏幕分辨率
    print_fonts(buf_back ,binfo->scrnx,8,8,COL8_WHITE,s);

    int mx,my;
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor(buf_mouse,99);                                                //在鼠标层写入图形数据

    layer_slide(lay_back,0,0);                                                //把背景层偏移搭到（0，0）
    layer_slide(lay_mouse,mx,my);                                            //把鼠标层偏移到中间位置
    layer_slide(lay_win,80,72);
    layer_updown(lay_back,0);                                                //设定背景层为0层
	layer_updown(lay_win,1);
    layer_updown(lay_mouse,2);                                                //设定鼠标层为1层

    sti();
    io_out8(PIC0_IMR, 0xf8); /* PIC1とキーボードを許可(11111001) */                    //开始接受鼠标和键盘中断
    io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */

    int kflag,mflag,tflag;                                                                //初始化键盘鼠标中断相关变量
    unsigned char mouse_dbuff[3];
    struct MOUSE_DEC mdec;
    init_keyboard();
    enable_mouse(&mdec);

    layer_refresh(lay_back,0,0,binfo->scrnx,48);

    extern struct TIMERCTRL timerctrl;
    for(;;){
        sprintf(s,"%010lu",timerctrl.count);
        boxfill8(buf_win,160,COL8_GREY,40,28,119,43);
        print_fonts(buf_win,160,40,28,COL8_BLACK,s);
        layer_refresh(lay_win,40,28,120,44);

        kflag = fifo8_status(&keyfifo);
        mflag = fifo8_status(&mousefifo);
        tflag = fifo8_status(&timerfifo);
        if( kflag || mflag || tflag){                                                //键盘或者鼠标的中断缓存有数据的时候进入
            cli();                                                             //屏蔽中断
            if(kflag)                                                           //键盘部分
            {
              unsigned char i = fifo8_get(&keyfifo);
              sti();
              char s[4];
              sprintf(s ,"%02X", i );
              boxfill8(buf_back, binfo->scrnx, COL8_BLACK, 0, 25, 15, 40);
              print_fonts(buf_back, binfo->scrnx, 0, 25, COL8_WHITE, s );
              layer_refresh(lay_back,0,25,15,40);
             }
            cli();
            if(mflag)                                                            //鼠标部分
            {
                unsigned char i = fifo8_get(&mousefifo);
                sti();
                if(mouse_decode(&mdec, i) != 0){                                //解码鼠标中断带来的数据
                    char s[30];
                    sprintf(s ,"%02X %02X %02X", mdec.buf[0],mdec.buf[1],mdec.buf[2]);
                    boxfill8(buf_back, binfo->scrnx, COL8_BLACK, 32, 25, 32+8*8-1, 40);
                    print_fonts(buf_back, binfo->scrnx, 32, 25, COL8_WHITE, s );  //先输出RAW数据压压惊
                    layer_refresh(lay_back,32,25,32+8*8,41);  //鼠标三个按键的解码

                    sprintf(s, "[lcr,%4d,%4d]",mdec.x,mdec.y);
                    if((mdec.btn & 0x01) != 0)
                        s[1] = 'L';
                    if((mdec.btn & 0x02) != 0)
                        s[3] = 'R';
                    if((mdec.btn & 0x04) != 0)
                        s[2] = 'C';
                    boxfill8(buf_back, binfo->scrnx, COL8_BLACK, 32, 40, 32+15*8-1, 55);
                    print_fonts(buf_back, binfo->scrnx, 32, 40, COL8_WHITE, s );
                    layer_refresh(lay_back,32,40,32+15*8-1,55);  //鼠标三个按键的解码

                    mx += mdec.x;
                    my += mdec.y;
                    if(mx < 0) mx = 0;
                    if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
                    if(my < 0) my = 0;
                    if(my > binfo->scrny - 1) my = binfo->scrny - 1;              //鼠标（X,Y）解码
                    sprintf(s,"(%3d,%3d)",mx,my);
                    boxfill8(buf_back, binfo->scrnx, COL8_BLACK, 32, 55, 32+79, 70);
                    print_fonts(buf_back, binfo->scrnx, 32, 55, COL8_WHITE, s );  //输出（X,Y）偏移
                    layer_refresh(lay_back,32,55,32+79,70);
                    layer_slide(lay_mouse,mx,my);                          //偏移鼠标层以移动光标
                }
            }
            if(tflag){
                 unsigned char i = fifo8_get(&timerfifo);
                 sti();
                 print_fonts(buf_back, binfo->scrnx, 170, 40, COL8_BLACK, "10[sec]");
                 layer_refresh(lay_back, 170, 40, 170 + 56 , 40 + 16);
            }
        }
        else
        sti();
    }
}





