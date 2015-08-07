#ifndef TIME_H_H
#define TIME_H_H

struct TIMERCTRL{
    unsigned long count;
}timerctrl;

#define PIT_CTRL  0x0043
#define PIT_CNT0  0x0040

void init_pit(void);

#endif
