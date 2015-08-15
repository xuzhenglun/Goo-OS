#include "mtask.h"
#include "timer.h"

struct TIMER *mt_timer;
int mt_tr;

void mt_init(void){
    mt_timer = timer_alloc();
    timer_settime(mt_timer,2);
    mt_tr = 3<<3;
}

void mt_taskswitch(){
    if(mt_tr == 3<<3){
        mt_tr = 4<<3;
    }else{
        mt_tr = 3<<3;
    }
    timer_settime(mt_timer,2);
    farjump(0,mt_tr);
    return;
}
