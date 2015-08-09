#include "timer.h"
#include "basic.h"

void  init_pit(void){
    io_out8(PIT_CTRL,0x34);
    io_out8(PIT_CNT0,0x9c);
    io_out8(PIT_CNT0,0x2e);
    timerctrl.count = 0;
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
     timer->timeout = timeout;
     timer->flags   = TIMER_FLAGS_USING;
}
