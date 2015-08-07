#include "timer.h"
#include "basic.h"

extern struct TIMERCTRL timerctrl;

void  init_pit(void){
    io_out8(PIT_CTRL,0x34);
    io_out8(PIT_CNT0,0x9c);
    io_out8(PIT_CNT0,0x2e);
    timerctrl.count = 0;
}
