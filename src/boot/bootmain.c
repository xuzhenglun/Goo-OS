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
#include "keyboard.h"

struct LAYER_CTL * layctl;                     //初始化定义层控制体
struct LAYER *lay_back,*lay_mouse,*lay_win;        //定义鼠标层和背景层
struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR; //内存管理块的内存位置

void bootmain(void) {
    struct BOOTINFO *binfo;                        //从内存中找到在IPL中保存的图形参数
    binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40];                                    //文字输出缓存，（在栈中）

    init_palette();
    init_gdtidt();
    init_pic();
    init_pit();


    char *buf_back,*buf_mouse,*buf_win;
    unsigned char keybuf[32],mousebuf[128],timerbuf[32];                //鼠标和键盘中断缓存，（在栈中）
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

    struct TASK * INIT = task_init(memman);
    task_set_priority(INIT,6);
    fifo8_taskwaker(&mousefifo,INIT);

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

    /*键盘*/
    extern char keytable0[0x80];
    extern char keytable1[0x80];

    int Key_to = 0;
    int Key_shift = 0;
    int Key_leds = (binfo->leds >> 4) & 0x0111;
    unsigned char Keycmd_wait = 0xff;
    struct FIFO8 keycmd;
    unsigned char keycmd_buf[32];
    fifo8_init(&keycmd, 32, keycmd_buf);
    fifo8_put(&keycmd, KEYCMD_LED);
    fifo8_put(&keycmd, Key_leds);

    /* main主循环部分变量初始化 */
    int kflag,mflag,tflag;                                                                //初始化键盘鼠标中断相关变量
    int x = 8;
    int color;

    /* task_console */
    struct TASK *task_cons;
    unsigned char *buf_task_cons;
    struct LAYER *task_cons_lay;
    task_cons = task_alloc(2, (int)&task_cons_main, 1, 64);
    task_cons_lay = layer_alloc(layctl);
    buf_task_cons = (unsigned char *) mem_alloc_4k(memman, 256 * 165);
    layer_setbuf(task_cons_lay, buf_task_cons, 256,165, -1);
    make_window8(buf_task_cons, 256,165, "Console", 0);
    make_textbox(task_cons_lay,8,28,240,128,COL8_BLACK);
    *((int *) (task_cons->tss.esp + 4)) = (int) task_cons_lay;
    task_run(task_cons);
    layer_slide(task_cons_lay, 178,  56);
    layer_updown(task_cons_lay, 1);

    for(;;){
        unsigned long overflow = -0x100;
        if(timerctrl.count >= overflow){
            timer_refresh();
            }//时间计数溢出，重新刷新时间。32位long为4字节，大约一年溢出。若编译器将其处理成8字节，我觉得没必要刷新了。

        if(fifo8_status(&keycmd) && Keycmd_wait == 0xff){
             Keycmd_wait = fifo8_get(&keycmd);
             io_out8(PORT_KEYDAT, KEYCMD_LED);
             wait_KBC_sendready();
             io_out8(PORT_KEYDAT, Keycmd_wait);
        }

        if(color < 0){
            boxfill8(lay_win->buf, lay_win->bxsize, COL8_WHITE, x, 28, x + 8, 44);
            layer_refresh(lay_win, x, 28, x + 8, 44);
        }

        cli();
        kflag = fifo8_status(&keyfifo);
        mflag = fifo8_status(&mousefifo);
        tflag = fifo8_status(&timerfifo);
        sti();
        if( kflag == 0 && mflag == 0 && tflag == 0){
            sti();
            task_sleep(INIT);
        }
        else{                                  //键盘或者鼠标的中断缓存有数据的时候进入
            if(kflag)                                                           //键盘部分
            {
                cli();
                unsigned char i = fifo8_get(&keyfifo);
                sti();
                sprintf(s ,"%02X", i );
                print_refreshable_font(lay_back,0,25,COL8_WHITE,COL8_LD_BLUE,s);
                if(0 <= i && i <= 0xFF){
                    if( i < 0x80){
                        if(Key_shift == 0){
                            s[0] = keytable0[i];
                            if((Key_leds & (1 << 2) ) != 0 && s[0] >= 'a' && s[0] <= 'z')
                                s[0] -= 'a' -'A';
                        }else{
                            s[0] = keytable1[i];
                        }
                    }else{
                        s[0] =  0;
                    }
                    if(s[0] != 0){
                        if(Key_to == 0){
                            if(x < 0x90){
                                s[1] = '\0';
                                print_refreshable_font(lay_win, x, 28, COL8_BLACK, COL8_WHITE, s);
                                x += 8;
                            }
                        }else{
                            fifo8_put(&task_cons->kfifo, s[0]);
                        }
                    }
                    if( i == 0x0e){
                        if(Key_to == 0){
                            if(x > 8){
                                print_refreshable_font(lay_win, x, 28, COL8_BLACK, COL8_WHITE, " ");
                                x -= 8;
                            }
                        }else{
                             fifo8_put(&task_cons->kfifo, 8); //8在ASCII中代表backspace
                        }
                    }
                    if( i == 0x0f ){
                        if( Key_to == 0 ){
                            Key_to = 1;
                            make_wtitle8(buf_win, lay_win->bxsize, "TASK_A", 0);
                            make_wtitle8(buf_task_cons, task_cons_lay->bxsize, "CONSOLE", 1);
                            color = -1;
                            boxfill8(lay_win->buf, lay_win->bxsize, COL8_WHITE, x, 28, x + 8, 44);
                            task_cons->now = 1;
                        }else{
                            Key_to = 0;
                            make_wtitle8(buf_win, lay_win->bxsize, "TASK_A", 1);
                            make_wtitle8(buf_task_cons, task_cons_lay->bxsize, "CONSOLE", 0);
                            color = COL8_BLACK;
                            task_cons->now = 0;
                            task_wake(task_cons);
                        }
                        layer_refresh(lay_win, 0, 0, lay_win->bxsize, 21);
                        layer_refresh(task_cons_lay, 0, 0, task_cons_lay->bxsize, 21);
                    }
                    if( i == 0x2a || i == 0x36)
                        Key_shift = 1;
                    if( i == 0xaa || i == 0xb6)
                        Key_shift = 0;
                    if( i == 0x3a ){
                        Key_leds ^= 1 << 2;
                        fifo8_put(&keycmd, Key_leds);
                   }
                    if( i == 0x45 ){
                        Key_leds ^= 1 << 1;
                        fifo8_put(&keycmd, Key_leds);
                    }
                    if( i == 0x46 ){
                        Key_leds ^= 1;
                        fifo8_put(&keycmd, Key_leds);
                    }
                    if( i == 0xfa ){
                        Keycmd_wait = 0xff;
                    }
                    if( i == 0xfe ){
                        wait_KBC_sendready();
                        io_out8(PORT_KEYDAT, Keycmd_wait);
                    }
                    if( i == 0x1c && Key_to != 0 ){
                        fifo8_put(&task_cons->kfifo, 10);
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

                    struct LAYER *mlayer;
                    if(Key_to == 0)
                        mlayer = lay_win;
                    else
                        mlayer = task_cons_lay;

                    if((mdec.btn & 0x01) != 0){
                        layer_slide(mlayer,mx - 80,my - 8);
                    }
                }
            }
            if(tflag){
                cli();
                unsigned char i = fifo8_get(&timerfifo);
                sti();
                if(i == 10){
                    print_refreshable_font(lay_back,170,28,COL8_WHITE,COL8_LD_BLUE,"10[sec]");
                }
                if(i == 3){
                    print_refreshable_font(lay_back,170,28,COL8_WHITE,COL8_LD_BLUE,"3[sec]");
                    timer_settime(timer2, 300);
                }
                if( i <= 1){
                    if (i == 1) {
                        timer_init(timer3, &timerfifo, 0);
                        if(color >= 0){
                             color = COL8_BLACK;
                        }
                    }else {
                        timer_init(timer3, &timerfifo, 1);
                        if(color >= 0){
                             color = COL8_WHITE;
                        }
                    }
                    timer_settime(timer3, 50);
                    if(color >= 0){
                    boxfill8(buf_win, lay_win->bxsize, color, x, 28, x + 8, 28 + 16);
                    layer_refresh(lay_win, x, 28, x + 8, 28 + 16);
                    }
                }
            }
        }
    }
}

void task_cons_main(struct LAYER *layer){
    struct FIFO8 tfifo;
    struct TIMER *timer_put;
    int i,color;
    unsigned char tfifobuf[32],kfifobuf[128];
    char s[12];
    int x,y;
    struct TASK *task = task_now();

    fifo8_init(&tfifo, 32, tfifobuf);
    fifo8_init(&task->kfifo, 128,kfifobuf);
    timer_put = timer_alloc();
    timer_init(timer_put, &tfifo, 1);
    timer_settime(timer_put, 50);
    fifo8_taskwaker(&tfifo,task);
    fifo8_taskwaker(&task->kfifo,task);

    x = 8 + 8;
    y = 28;

    print_refreshable_font(layer,8,28,COL8_WHITE,COL8_BLACK,">");
    layer_refresh(layer, 8, 28, 8 + 8, 28 + 16);

    while(1){
        cli();
        if(fifo8_status(&tfifo) == 0 && fifo8_status(&task->kfifo) == 0){
            sti();
            task_sleep(task_now());
        }else{
            i = fifo8_get(&tfifo);
            sti();
            if( i <= 1 && task->now == 1){
                if(i == 1){
                    timer_init(timer_put,&tfifo,0);
                    color = COL8_WHITE;
                }else{
                    timer_init(timer_put,&tfifo,1);
                    color = COL8_BLACK;
                }
            }
            timer_settime(timer_put, 50);

            cli();
            i = fifo8_get(&task->kfifo);
            sti();
            if( 0 <= i && i <= 0xff){
                switch(i){
                    case 8:
                        if(x > 16){
                            print_refreshable_font(layer, x, y, COL8_BLACK, COL8_BLACK, " ");
                            x  -= 8;
                        }
                        break;
                    case 10:
                        boxfill8(layer->buf, layer->bxsize, COL8_BLACK, x, y, x + 8, y + 16);
                        if(y < 28 + 112){
                            y += 16;
                        }else{
                            for(y = 28; y < 28 + 112; y++){
                                for(x = 8; x < 8 + 240; x++){
                                    layer->buf[x + y * layer->bxsize] = layer->buf[x + (y + 16) * layer->bxsize];
                                }
                            }
                            for(y = 28 + 112; y < 28 + 128; y++){
                                for(x = 8; x < 8 + 240; x++){
                                    layer->buf[x + y * layer->bxsize] = COL8_BLACK;
                                }
                            }
                            layer_refresh(layer, 8 , 28, 8 + 240, 28 + 128);
                            y -= 16;
                        }
                        print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, ">");
                        x = 16;
                        break;
                    default:
                        if( x < 240 ){
                            s[0] = i;
                            s[1] = '\0';
                            print_refreshable_font(layer, x, y, COL8_WHITE, COL8_BLACK, s);
                            x += 8;
                        }
                }
            }
            boxfill8(layer->buf, layer->bxsize, color, x, y, x + 8, y + 16);
            layer_refresh(layer, x, y, x + 8, y + 16);
        }
        if(task->now == 0 && color != COL8_BLACK){
            color = COL8_BLACK;
            boxfill8(layer->buf, layer->bxsize, COL8_BLACK, x, y, x + 8, y + 16);
            layer_refresh(layer, x, y, x + 8, y + 16);
            task->now = -1;
        }
    }
}
