#ifndef FIFO_H_H
#define	FIFO_H_H

#include "mtask.h"

struct FIFO8{
    unsigned char *buf;
    int head,tail,size,flags;
    struct TASK *task;
}keyfifo,mousefifo;

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int  fifo8_put (struct FIFO8 *fifo, unsigned char data);
int  fifo8_get (struct FIFO8 *fifo);
int  fifo8_status(struct FIFO8 *fifo);
void fifo8_taskwaker(struct FIFO8 *fifo,struct TASK *task);

#endif
