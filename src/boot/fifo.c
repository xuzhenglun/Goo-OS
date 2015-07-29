#include "fifo.h"

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf){
    fifo->size = size;
    fifo->buf  = buf;
    fifo->flags= 0;
    fifo->head = 0;
    fifo->tail = 0;
    return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data){
    if((fifo->tail - fifo->head + 1 + fifo->size ) % fifo->size == 0){
        fifo->flags = 1;
        return -1;
    }
    else{
    fifo->buf[fifo->tail++] = data;
    if(fifo->tail == fifo->size)
        fifo->tail = 0;
    return 0;
    }
}

int fifo8_get(struct FIFO8 *fifo){
    unsigned char data;
    if(fifo->head == fifo->tail){
        return -1;
    }
    else{
        data = fifo->buf[fifo->head++];
         if(fifo->head == fifo->size)
             fifo->head = 0;
         return data;
    }
}

int fifo8_status(struct FIFO8 *fifo){
    return (fifo->tail - fifo->head + fifo->size ) % fifo->size;
}
