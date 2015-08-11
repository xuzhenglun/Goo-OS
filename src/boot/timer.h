#ifndef TIMER_H_H
#define TIMER_H_H

#define PIT_CTRL  0x0043
#define PIT_CNT0  0x0040
#define MAX_TIMER 500
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

struct TIMER{
    unsigned int  timeout,flags;
    struct FIFO8 *fifo;
    unsigned char data;
};

struct TIMERCTRL{
    unsigned long count,next;
    int using;
    struct TIMER *timer_p[MAX_TIMER];
    struct TIMER timer[MAX_TIMER];
}timerctrl;


void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data);
void timer_settime(struct TIMER *timer,unsigned int timeout);
void timer_refresh(void);

#endif
