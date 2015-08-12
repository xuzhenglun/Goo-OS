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

    char keybuf[32],mousebuf[128],timerbuf[32];                //鼠标和键盘中断缓存，（在栈中）
    struct FIFO8 timerfifo;
    struct TIMER *timer,*timer2,*timer3;
    fifo8_init(&keyfifo,   32,  keybuf  );
    fifo8_init(&mousefifo, 128, mousebuf);

    fifo8_init(&timerfifo, 32, timerbuf);
    timer = timer_alloc();
    timer_init(timer, &timerfifo, 10);
    timer_settime(timer, 1000);
    timer2 = timer_alloc();
    timer_init(timer2, &timerfifo, 3);
    timer_settime(timer2, 300);
    timer3 = timer_alloc();
    timer_init(timer3, &timerfifo, 1);
    timer_settime(timer3, 50);

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
        print_refreshable_font(lay_win,40,28,COL8_BLACK,COL8_GREY,s);

        unsigned long overflow = -0x100;
        if(timerctrl.count >= overflow){
            timer_refresh();
            }//时间计数溢出，重新刷新时间。32位long为4字节，大约一年溢出。若编译器将其处理成8字节，我觉得没必要刷新了。

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
              print_refreshable_font(lay_back,0,25,COL8_WHITE,COL8_LD_BLUE,s);
             }
            cli();
            if(mflag)                                                            //鼠标部分
            {
                unsigned char i = fifo8_get(&mousefifo);
                sti();
                if(mouse_decode(&mdec, i) != 0){                                //解码鼠标中断带来的数据
                    char s[30];
                    sprintf(s ,"%02X %02X %02X", mdec.buf[0],mdec.buf[1],mdec.buf[2]);
                    print_refreshable_font(lay_back,32,24,COL8_WHITE,COL8_LD_BLUE,s);

                    sprintf(s, "[lcr,%4d,%4d]",mdec.x,mdec.y);
                    if((mdec.btn & 0x01) != 0)
                        s[1] = 'L';
                    if((mdec.btn & 0x02) != 0)
                        s[3] = 'R';
                    if((mdec.btn & 0x04) != 0)
                        s[2] = 'C';
                    print_refreshable_font(lay_back,32,40,COL8_WHITE,COL8_LD_BLUE,s);

                    mx += mdec.x;
                    my += mdec.y;
                    if(mx < 0) mx = 0;
                    if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
                    if(my < 0) my = 0;
                    if(my > binfo->scrny - 1) my = binfo->scrny - 1;              //鼠标（X,Y）解码
                    sprintf(s,"(%3d,%3d)",mx,my);
                    print_refreshable_font(lay_back,32,55,COL8_WHITE,COL8_LD_BLUE,s);
                    layer_slide(lay_mouse,mx,my);                          //偏移鼠标层以移动光标
                }
            }
            if(tflag){
                    unsigned char i = fifo8_get(&timerfifo);
                    sti();
                if(i == 10){
                    print_refreshable_font(lay_back,170,24,COL8_WHITE,COL8_LD_BLUE,"10[sec]");
                }
                if(i == 3){
                    print_refreshable_font(lay_back,170,40,COL8_WHITE,COL8_LD_BLUE,"3[sec]");
                }
                if(i == 0 || i == 1){
                    if (i != 0) {
                        timer_init(timer3, &timerfifo, 0);
                        boxfill8(buf_back, binfo->scrnx, COL8_BLACK, 170, 40+16, 177, 40+16+14);
                    }else {
                        timer_init(timer3, &timerfifo, 1);
                        boxfill8(buf_back, binfo->scrnx, COL8_LD_BLUE, 170, 40+16, 177, 40+16+14);
                    }
                    timer_settime(timer3, 50);
                    layer_refresh(lay_back, 170, 40+16, 177, 40+16+14);
                }
            }
        else
        sti();
         }
    }
}





