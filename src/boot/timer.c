#include "timer.h"
#include "basic.h"

void  init_pit(void){
    io_out8(PIT_CTRL,0x34);
    io_out8(PIT_CNT0,0x9c);
    io_out8(PIT_CNT0,0x2e);
    timerctrl.count = 0;
    timerctrl.next  = -1;
    timerctrl.using = 0;
    for(int i = 0; i < MAX_TIMER; i++){
        timerctrl.timer[i].flags = 0;
    }
}

struct TIMER *timer_alloc(void){
    for(int i = 0; i < MAX_TIMER; i++){
        if(timerctrl.timer[i].flags == 0){
            timerctrl.timer[i].flags = TIMER_FLAGS_ALLOC;
            return &timerctrl.timer[i];
        }
    }
    return 0;
}

void timer_free(struct TIMER *timer){
     timer->flags = 0;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data){
    timer->fifo = fifo;
    timer->data = data;
}

void timer_settime(struct TIMER *timer,unsigned int timeout){
     timer->timeout = timeout + timerctrl.count;
     timer->flags   = TIMER_FLAGS_USING;
     int eflags = io_load_eflags();
     cli();
     int i;
     for(i = 0;i < timerctrl.using; i++){
         if(timerctrl.timer_p[i]->timeout >= timer->timeout){
            break;
         }
    }
     for(int j = timerctrl.using; j > i; j--){
          timerctrl.timer_p[j] = timerctrl.timer_p[j - 1];
     }
     timerctrl.using++;
     timerctrl.timer_p[i] = timer;
     timerctrl.next = timerctrl.timer_p[0]->timeout;
    io_store_eflags(eflags);
}

void timer_refresh(void){
     int t = timerctrl.count;
     cli();
     timerctrl.count -= t;
     for(int i = 0; i < MAX_TIMER; i++){
         if(timerctrl.timer[i].flags == TIMER_FLAGS_USING){
             timerctrl.timer[i].timeout -= t;
         }
     }
     sti();
}
