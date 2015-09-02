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
    int x,y;
    struct TASK *task = task_now();
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);

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

    int *fat = (int *)mem_alloc_4k(memman, sizeof(int) * 2880);
    file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));

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
                        layer_refresh(layer, x, y, x + 8, y + 16);
                        cmdline[x / 8 - 2] = '\0';
                        y = cons_newline(y, layer);
                        if(!strcmp(cmdline,"mem")){
                            int memtotal = memtest(0x00300000, 0xbfffffff); //总内存大小
                            sprintf(s,"MEMORY:%dMB|FREE:%dKB",memtotal /(1024*1024),mem_total(memman)/1024);
                            print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, s);
                            y = cons_newline(y, layer);
                            y = cons_newline(y, layer);
                        }else if(!strcmp(cmdline,"clear")){
                            for(int i = 28; i < 28 + CON_TEXT_Y; i++){
                                for(int t = 8; t < 8 + CON_TEXT_X; t++){
                                    layer->buf[t + i * layer->bxsize] = COL8_BLACK;
                                }
                            }
                            layer_refresh(layer, 8, 28, 8 + CON_TEXT_X, 28 + CON_TEXT_Y);
                            y = 28;
                        }else if(!strcmp(cmdline,"ls")){
                            /*sprintf(s,"%s",finfo[0].name);*/
                            /*print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, s);*/
                            for(int x = 0; x < 224 && finfo[x].filename.name[0] != '\0';  x++){
                                /*if (finfo[x].name[0] == 0x00)*/
                                    /*break;*/
                                if (finfo[x].filename.name[0] != 0xe5){
                                    if((finfo[x].type & 0x18) == 0){
                                        sprintf(s, "filename.ext    %7d", finfo[x].size);
                                        for(int y = 0; y < 8; y++){
                                            s[y] = finfo[x].filename.name[y];
                                        }
                                        s[9] = finfo[x].filename.ext[0];
                                        s[10] = finfo[x].filename.ext[1];
                                        s[11] = finfo[x].filename.ext[2];
                                        print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, s);
                                        y = cons_newline(y, layer);
                                    }
                                }
                            }
                            y = cons_newline(y, layer);
                        }else if(!strncmp(cmdline, "cat", 3)){
                            struct FILENAME file;
                            memcpy(&file,&"           ",sizeof(struct FILENAME));
                            int index = 0;
                            int flag = 0;
                            for(int i = 4; cmdline[i] != '\0' && i < 30 && index < 11; i++){
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
                            int cursor_x = 8;
                            if(fileid != -1){
                                int filesize = finfo[fileid].size;
                                /*char *p = (char *)(finfo[fileid].clustno * 512 + 0x003e00 + ADR_DISKIMG);*/
                                char *p = (char *) mem_alloc_4k(memman, filesize);
                                file_loadfile(finfo[fileid].clustno, filesize, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
                                for(int i = 0; i < filesize ; i++){
                                    /*sprintf(s,"%X",&p[i]);*/
                                    /*print_refreshable_font(layer, 0, 0, COL8_WHITE, COL8_BLACK,s);*/
                                    if(p[i] == '\r' );
                                    else if(p[i] == '\n'){
                                        y = cons_newline(y, layer);
                                        cursor_x = 8;
                                    }else if(p[i] == '\t'){
                                        cursor_x += 8 * 4;
                                        if(cursor_x >= 8 + CON_TEXT_X){
                                            cursor_x = 8;
                                            y = cons_newline(y, layer);
                                        }
                                    }else{
                                        s[0] = p[i];
                                        s[1] = '\0';
                                        print_refreshable_font(layer, cursor_x, y, COL8_WHITE, COL8_BLACK, s);
                                        cursor_x += 8;
                                }
                                    if(cursor_x == 8 + CON_TEXT_X){
                                        cursor_x = 8;
                                        y = cons_newline(y, layer);
                                    }
                                }
                                mem_free(memman, (int)p, filesize);
                            }else{
                                print_refreshable_font(layer, cursor_x, y, COL8_WHITE, COL8_BLACK, "No Such File");
                            }
                            y = cons_newline(y, layer);
                        }else if(cmdline[0] != '\0'){
                            sprintf(s,"No Such Command \"%s\".",cmdline);
                            print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, s);
                            y = cons_newline(y, layer);
                            y = cons_newline(y, layer);
                        }
                        print_refreshable_font(layer, 8, y, COL8_WHITE, COL8_BLACK, ">");
                        x = 16;
                        break;
                    default:
                        if( x < CON_TEXT_X ){
                            s[0] = i;
                            s[1] = '\0';
                            if(x / 8 - 2 < 30 - 1){  //边界检查，否则溢出崩溃
                                cmdline[x / 8 - 2] = i;
                            };
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

int cons_newline(int cursor_y, struct LAYER *layer){
    if(cursor_y <= 28 + CON_TEXT_Y - 16 * 2){
        cursor_y += 16;
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
    return cursor_y;
}
