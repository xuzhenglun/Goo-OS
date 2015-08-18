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
#include "mtask.h"

struct LAYER_CTL * layctl;                     //初始化定义层控制体
struct LAYER *lay_back,*lay_mouse,*lay_win;        //定义鼠标层和背景层
struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR; //内存管理块的内存位置

void bootmain(void) {
    static char keytable[0x54] = {
         0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 0,   0,   '\\', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
    };

    struct BOOTINFO *binfo;                        //从内存中找到在IPL中保存的图形参数
    binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40];                                    //文字输出缓存，（在栈中）

    init_palette();
    init_gdtidt();
    init_pic();
    init_pit();


    char *buf_back,*buf_mouse,*buf_win;
    char keybuf[32],mousebuf[128],timerbuf[32];                //鼠标和键盘中断缓存，（在栈中）
    struct FIFO8 timerfifo;
    fifo8_init(&keyfifo,   32,  keybuf  );
    fifo8_init(&mousefifo, 128, mousebuf);
    fifo8_init(&timerfifo, 32, timerbuf);

    struct TIMER *timer,*timer2,*timer3;
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
    memtotal = memtest(0x00300000, 0xbfffffff); //总内存大小
    mem_init(memman);                            //初始化内存控制块
    mem_free(memman, 0x00300000,memtotal - 0x00300000); //初始化内存页

    layctl = layer_ctl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);  //初始化层控制块

    task_init(memman);

    /* 背景层 */
    lay_back = layer_alloc(layctl);                                            //创建背景层
    buf_back = (unsigned char *) mem_alloc_4k(memman,binfo->scrnx * binfo->scrny);
    layer_setbuf(lay_back,buf_back,binfo->scrnx,binfo->scrny,-1);             //设定背景层
    init_screen8(buf_back, binfo->scrnx,binfo->scrny);                         //画一下桌面
    sprintf(s,"| MEMORY:%dMB|FREE:%dKB",memtotal /(1024*1024),mem_total(memman)/1024); //输出内存消息
    print_fonts(buf_back ,binfo->scrnx,100,8,COL8_WHITE,s);
    sprintf(s,"%d * %d ",binfo->scrnx,binfo->scrny);                                    //输出屏幕分辨率
    print_fonts(buf_back ,binfo->scrnx,8,8,COL8_WHITE,s);
    layer_slide(lay_back,0,0);                                                //把背景层偏移搭到（0，0）
    layer_updown(lay_back,0);                                                //设定背景层为0层

    /* 鼠标层 */
    lay_mouse = layer_alloc(layctl);                                        //创建鼠标层
    buf_mouse = (unsigned char *) mem_alloc_4k(memman,16*16);
    layer_setbuf(lay_mouse,buf_mouse,16,16,99);                                //设定鼠标层（透明色为99）
    init_mouse_cursor(buf_mouse,99);                                                //在鼠标层写入图形数据
    int mx,my;
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    layer_slide(lay_mouse,mx,my);                                            //把鼠标层偏移到中间位置
    layer_updown(lay_mouse,2);                                                //设定鼠标层为1层

    /* 窗口层 */
    lay_win   = layer_alloc(layctl);                                        //创建窗口层
    buf_win   = (unsigned char *) mem_alloc_4k(memman,160 * 52);
    layer_setbuf(lay_win  ,buf_win,160,52,-1);
    make_window8(buf_win,160,52,"TYPING",1);
    make_textbox(lay_win,8,28,144,16,COL8_WHITE);
    layer_slide(lay_win,8,56);
    layer_updown(lay_win,1);

    /* 鼠标键盘中断开 */
    sti();
    io_out8(PIC0_IMR, 0xf8); /* PIC1とキーボードを許可(11111001) */                    //开始接受鼠标和键盘中断
    io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
    unsigned char mouse_dbuff[3];
    struct MOUSE_DEC mdec;
    init_keyboard();
    enable_mouse(&mdec);

    /* main主循环部分变量初始化 */
    int kflag,mflag,tflag;                                                                //初始化键盘鼠标中断相关变量
    int x = 8;

    /* task_b * 3 */
    struct TASK *task_b[3];
    unsigned char *buf_task_b;
    struct LAYER *task_b_lay[3];
    for(int i = 0; i < 3; i++){
        task_b[i] = task_alloc(1, (int)&task_b_main, 1, 64);
        task_b_lay[i] = layer_alloc(layctl);
		buf_task_b = (unsigned char *) mem_alloc_4k(memman, 160 * 52);
		layer_setbuf(task_b_lay[i], buf_task_b, 160,52, -1); /* 透明色なし */
		sprintf(s, "task_b%d", i);
		make_window8(buf_task_b, 160,52, s, 0);
        *((int *) (task_b[i]->tss.esp + 4)) = (int) task_b_lay[i];
        task_run(task_b[i]);
    }
	layer_slide(task_b_lay[0], 178,  56);
	layer_slide(task_b_lay[1],   8, 116);
	layer_slide(task_b_lay[2], 178, 116);
    layer_updown(task_b_lay[0], 1);
	layer_updown(task_b_lay[1], 2);
	layer_updown(task_b_lay[2], 3);

    for(;;){
        unsigned long overflow = -0x100;
        if(timerctrl.count >= overflow){
            timer_refresh();
            }//时间计数溢出，重新刷新时间。32位long为4字节，大约一年溢出。若编译器将其处理成8字节，我觉得没必要刷新了。
        cli();
        kflag = fifo8_status(&keyfifo);
        mflag = fifo8_status(&mousefifo);
        tflag = fifo8_status(&timerfifo);
        sti();
        if( kflag || mflag || tflag){                                                //键盘或者鼠标的中断缓存有数据的时候进入
            if(kflag)                                                           //键盘部分
            {
                cli();
                unsigned char i = fifo8_get(&keyfifo);
                sti();
                sprintf(s ,"%02X", i );
                print_refreshable_font(lay_back,0,25,COL8_WHITE,COL8_LD_BLUE,s);
                if(0 <= i && i <= 511-256 && i < 0x54){
                    if(keytable[i] != 0 && x < 144){
                        s[0] = keytable[i];
                        s[1] = '\0';
                        print_refreshable_font(lay_win, x, 28, COL8_BLACK, COL8_WHITE, s);
                        x += 8;
                    }
                    if( i == 0x0e && x > 8 ){
                        print_refreshable_font(lay_win, x, 28, COL8_BLACK, COL8_WHITE, " ");
                        x -= 8;
                    }
                }
             }
            if(mflag)                                                            //鼠标部分
            {
                cli();
                unsigned char i = fifo8_get(&mousefifo);
                sti();
                if(mouse_decode(&mdec, i) != 0){                                //解码鼠标中断带来的数据
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

                    if((mdec.btn & 0x01) != 0){
                        layer_slide(lay_win,mx - 80,my - 8);
                    }
                }
            }
            if(tflag){
                cli();
                unsigned char i = fifo8_get(&timerfifo);
                sti();
                if(i == 10){
                    print_refreshable_font(lay_back,170,24,COL8_WHITE,COL8_LD_BLUE,"10[sec]");
                }
                if(i == 3){
                    print_refreshable_font(lay_back,170,40,COL8_WHITE,COL8_LD_BLUE,"3[sec]");
                }
                if( i <= 1){
                    if (i == 1) {
                        timer_init(timer3, &timerfifo, 0);
                        boxfill8(buf_win, lay_win->bxsize, COL8_BLACK, x, 28, x+6, 28+14);
                    }else {
                        timer_init(timer3, &timerfifo, 1);
                        boxfill8(buf_win, lay_win->bxsize, COL8_WHITE, x, 28, x+6, 28+14);
                    }
                    layer_refresh(lay_win, x, 28, x+6, 28+14);
                    timer_settime(timer3, 50);
                }
            }
        else
        sti();
         }
    }
}

void task_b_main(struct LAYER *lay_win_b){
    struct FIFO8 fifo;
    struct TIMER *timer_put;
    int i,fifobuf[128];
    char s[12];
    long count0,count = 0;

    fifo8_init(&fifo, 128, fifobuf);
    timer_put = timer_alloc();
    timer_init(timer_put, &fifo, 4);
    timer_settime(timer_put, 100);

    while(1){
        count++;
        cli();
        if(fifo8_status(&fifo) == 0){
             sti();
        }else
        {
            i = fifo8_get(&fifo);
            sti();
            if(i == 4){
                sprintf(s, "%u", count - count0);
                print_refreshable_font(lay_win_b, 34, 28, COL8_BLACK, COL8_GREY,s);
                count0 = count;
                timer_settime(timer_put, 100);
            }
        }
    }
}
