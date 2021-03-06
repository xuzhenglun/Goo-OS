#define MEMMAN_ADDR 0x00280000

#include "console.h"
#include "bootmain.h"
#include "graph.h"
#include "dsctbl.h"
#include "int.h"
#include "../golibc/stdio.h"
#include "../golibc/string.h"
#include "basic.h"
#include "memory.h"
#include "timer.h"
#include "mtask.h"
#include "keyboard.h"
#include "fat12.h"

void task_cons_main(struct LAYER *layer){
    struct FIFO8 tfifo;
    struct TIMER *timer_put;
    int i,color = 0;
    unsigned char tfifobuf[32],kfifobuf[128];
    char s[12],cmdline[30];
    struct TASK *task = task_now();

    fifo8_init(&tfifo, 32, tfifobuf);
    fifo8_init(&task->kfifo, 128,kfifobuf);
    timer_put = timer_alloc();
    timer_init(timer_put, &tfifo, 1);
    timer_settime(timer_put, 50);
    fifo8_taskwaker(&tfifo,task);
    fifo8_taskwaker(&task->kfifo,task);

    struct CONSOLE cons;
    cons.x = 8;
    cons.y = 28;
    cons.layer = layer;
    *((int *)0x0fec) = (int)&cons;

    cons_putchar(&cons,'>');

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
                        if(cons.x > 16){
                            print_refreshable_font(layer, cons.x, cons.y, COL8_BLACK, COL8_BLACK, " ");
                            cons.x  -= 8;
                        }
                        break;
                    case 10:
                        boxfill8(layer->buf, layer->bxsize, COL8_BLACK, cons.x, cons.y, cons.x + 8, cons.y + 16);
                        layer_refresh(layer, cons.x, cons.y, cons.x + 8, cons.y + 16);
                        cmdline[cons.x / 8 - 2] = '\0';
                        cons_newline(&cons);
                        cons.x = 8;
                        cons_runcmd(cmdline, &cons);
                        cons.x = 8;
                        cons_putchar(&cons,'>');
                        break;
                    default:
                        if( cons.x < CON_TEXT_X ){
                            s[0] = i;
                            s[1] = '\0';
                            if(cons.x / 8 - 2 < 30 - 1){  //边界检查，否则溢出崩溃
                                cmdline[cons.x / 8 - 2] = i;
                            };
                            print_refreshable_font(layer, cons.x, cons.y, COL8_WHITE, COL8_BLACK, s);
                            cons.x += 8;
                        }
                }
            }
            boxfill8(layer->buf, layer->bxsize, color, cons.x, cons.y, cons.x + 8, cons.y + 16);
            layer_refresh(layer, cons.x, cons.y, cons.x + 8, cons.y + 16);
        }
        if(task->now == 0 && color != COL8_BLACK){
            color = COL8_BLACK;
            boxfill8(layer->buf, layer->bxsize, COL8_BLACK, cons.x, cons.y, cons.x + 8, cons.y + 16);
            layer_refresh(layer, cons.x, cons.y, cons.x + 8, cons.y + 16);
            task->now = -1;
        }
    }
}

void cons_newline(struct CONSOLE *console){
    struct LAYER *layer = console->layer;
    if(console->y <= 28 + CON_TEXT_Y - 16 * 2){
        console->y += 16;
    }else{
        for(int y = 28; y < 28 + CON_TEXT_Y - 16; y++){
            for(int x = 8; x < 8 + CON_TEXT_X; x++){
                layer->buf[x + y * layer->bxsize] = layer->buf[x + (y + 16) * layer->bxsize];
            }
        }
        for(int y = 28 + CON_TEXT_Y - 16; y < 28 + CON_TEXT_Y; y++){
            for(int x = 8; x < 8 + CON_TEXT_X; x++){
                layer->buf[x + y * layer->bxsize] = COL8_BLACK;
            }
        }
        layer_refresh(layer, 8 , 28, 8 + CON_TEXT_X, 28 + CON_TEXT_Y);
    }
}

int find_file(char *cmdline, int start){
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);
    struct FILENAME file;
    memcpy(&file,&"           ",sizeof(struct FILENAME));
    int index = 0;
    int flag = 0;
    for(int i = start; cmdline[i] != '\0' && i < 30 && index < 11; i++){
        if(cmdline[i] == '.'){
            index = 8;
            flag = 1;
        }
        if(cmdline[i] != ' ' && cmdline[i] != '.'){
            if(cmdline[i] <= 'z' && cmdline[i] >= 'a')
               cmdline[i] = cmdline[i] - ('a' - 'A');
            if(flag == 0 && index < 8)
                file.name[index] = cmdline[i];
            else
                file.ext[index - 8] = cmdline[i];
            if((flag == 0 && index < 8) || (flag == 1 && index >= 8))
                index++;
        }
    }
    int fileid = -1;
    for(int i = 0; i < 224; i++){
        if(!memcmp(&file,&finfo[i].filename,sizeof(struct FILENAME))){
            fileid = i;
            break;
        }
    }
    return fileid;
}

void cons_runcmd(char *cmdline, struct CONSOLE *console){
    struct LAYER * layer = console->layer;
    char s[30];
    if(!strcmp(cmdline,"mem")){
        cons_mem(console);
    }else if(!strcmp(cmdline,"clear")){
        cons_clear(console);
    }else if(!strcmp(cmdline,"ls")){
        cons_ls(console);
    }else if(!strncmp(cmdline, "cat", 3)){
        cons_cat(cmdline,console);
    }else if(!strncmp(cmdline, "start",5)){
        cons_startapp(cmdline,console);
    }else if(cmdline[0] != '\0'){
        sprintf(s,"No Such Command \"%s\".",cmdline);
        print_refreshable_font(layer, 8, console->y, COL8_WHITE, COL8_BLACK, s);
        cons_newline(console);
        cons_newline(console);
    }
}

void cons_mem(struct CONSOLE *console){
    char s[30];
    struct LAYER *layer = console->layer;
    int memtotal = memtest(0x00300000, 0xbfffffff); //总内存大小
    sprintf(s,"MEMORY:%dMB|FREE:%dKB",memtotal /(1024*1024),mem_total(memman)/1024);
    print_refreshable_font(layer, 8, console->y, COL8_WHITE, COL8_BLACK, s);
    cons_newline(console);
    cons_newline(console);
}

void cons_clear(struct CONSOLE *console){
    struct LAYER *layer = console->layer;
    for(int i = 28; i < 28 + CON_TEXT_Y; i++){
        for(int t = 8; t < 8 + CON_TEXT_X; t++){
            layer->buf[t + i * layer->bxsize] = COL8_BLACK;
        }
    }
    layer_refresh(layer, 8, 28, 8 + CON_TEXT_X, 28 + CON_TEXT_Y);
    console->y = 28;
}

void cons_ls(struct CONSOLE *console){
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);
    struct LAYER *layer = console->layer;
    char s[30];
    for(int x = 0; x < 224 && finfo[x].filename.name[0] != '\0';  x++){
        if (finfo[x].filename.name[0] != 0xe5){
            if((finfo[x].type & 0x18) == 0){
                sprintf(s, "filename.ext    %7d", finfo[x].size);
                for(int y = 0; y < 8; y++){
                    s[y] = finfo[x].filename.name[y];
                }
                s[9] = finfo[x].filename.ext[0];
                s[10] = finfo[x].filename.ext[1];
                s[11] = finfo[x].filename.ext[2];
                print_refreshable_font(layer, 8, console->y, COL8_WHITE, COL8_BLACK, s);
                cons_newline(console);
            }
        }
    }
    cons_newline(console);
}

void cons_cat(char *cmdline, struct CONSOLE *console){
    int fileid = find_file(cmdline,4);
    int cursor_x = 8;
    struct LAYER *layer = console->layer;
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    int *fat = (int *)mem_alloc_4k(memman, sizeof(int) * 2880);
    file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));

    if(fileid != -1){
        int filesize = finfo[fileid].size;
        char *p = (char *) mem_alloc_4k(memman, finfo[fileid].size);
        file_loadfile(finfo[fileid].clustno, finfo[fileid].size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
        cons_print(console, p, filesize);
        mem_free(memman, (int)p, filesize);
    }else{
        print_refreshable_font(layer, cursor_x, console->y, COL8_WHITE, COL8_BLACK, "No Such File");
    }
    cons_newline(console);
}

void cons_startapp(char *cmdline, struct CONSOLE *console){
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
    struct LAYER *layer = console->layer;
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    int *fat = (int *)mem_alloc_4k(memman, sizeof(int) * 2880);
    file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));
    struct TASK *task = task_now();

    int fileid = find_file(cmdline,6);
    if(fileid != -1){
        char *p = (char *)mem_alloc_4k(memman, finfo[fileid].size);
        char *q = (char *)mem_alloc_4k(memman, 64 * 1024);
        *((int *)0xfe8) = (int)q;
        file_loadfile(finfo[fileid].clustno, finfo[fileid].size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
        set_segmdesc(gdt + 1003, finfo[fileid].size - 1, (int)p, AR_CODE32_ER + 0x60);
        set_segmdesc(gdt + 1004, 64 * 1024 - 1         , (int)q, AR_DATA32_RW + 0x60);
        start_app(0, 1003 << 3, 64 * 1024, 1004 << 3, &(task->tss.esp0));
        mem_free(memman, (int)p, finfo[fileid].size);
        mem_free(memman, (int)q, 64 * 1024);
    }else{
        print_refreshable_font(layer, 8, console->y, COL8_WHITE, COL8_BLACK, "No Such File");
    }
    cons_newline(console);
}

void cons_print(struct CONSOLE *console, char *p, int filesize){
    char s[30];
    for(int i = 0; i < filesize ; i++){
        if(p[i] == '\r' );
        else if(p[i] == '\n'){
            cons_newline(console);
            console->x = 8;
        }else if(p[i] == '\t'){
            console->x += 8 * 4;
            if(console->x == 8 + CON_TEXT_X){
                console->x = 8;
                cons_newline(console);
            }
        }else{
            s[0] = p[i];
            s[1] = '\0';
            print_refreshable_font(console->layer, console->x, console->y, COL8_WHITE, COL8_BLACK, s);
            console->x += 8;
        }
        if(console->x == 8 + CON_TEXT_X){
            console->x = 8;
            cons_newline(console);
        }
    }
}

void cons_putchar(struct CONSOLE *console, int chr){
    cons_print(console,(char *)&chr,1);
}

void cons_puts(struct CONSOLE *console, char *p){
    int i = 0;
    while(p[i]){
        cons_putchar(console, p[i]);
        i++;
    }
}
