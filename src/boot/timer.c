#include "timer.h"
#include "basic.h"

void  init_pit(void){
    io_out8(PIT_CTRL,0x34);
    io_out8(PIT_CNT0,0x9c);
    io_out8(PIT_CNT0,0x2e);
    timerctrl.count = 0;
    timerctrl.next  = -1;
    for(int i = 0; i < MAX_TIMER; i++){
        timerctrl.timer[i].flags = 0;
    }
    timerctrl.head = timer_alloc();
    timerctrl.head->timeout = -1;
    timerctrl.head->flags   = TIMER_FLAGS_USING;
    timerctrl.head->next    = -1;
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
    if(timer->flags == TIMER_FLAGS_USING){
        remove_timer(timer);
    }
     timer->timeout = timeout + timerctrl.count;
     timer->flags   = TIMER_FLAGS_USING;
     int eflags = io_load_eflags();
     cli();
     struct TIMER *head = timerctrl.head;
     if(timer->timeout <= head->timeout){
         timerctrl.head = timer;
         timer->next    = head;
         timerctrl.next = timer->timeout;
         io_store_eflags(eflags);
         return;
     }
     struct TIMER * tmp = head;
     while(timer->timeout > tmp->next->timeout){
         tmp = tmp->next;
     }
     timer->next = tmp->next;
     tmp->next   = timer;
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

void remove_timer(struct TIMER *timer){
    struct TIMER *tmp = timerctrl.head;
    if(timerctrl.head == timer){
        timerctrl.head->flags = TIMER_FLAGS_ALLOC;
        timerctrl.head = timerctrl.head->next;
    }else{
        while(tmp->next != timer){
             tmp = tmp->next;
        }
        tmp->next = timer->next;
        timer->flags = TIMER_FLAGS_ALLOC;
    }
}
