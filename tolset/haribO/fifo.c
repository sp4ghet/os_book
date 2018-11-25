#include "fifo.h"

void fifo8_init(struct FIFO *fifo, int size, unsigned char *buf){
    fifo->buf = buf;
    fifo->size = size;
    fifo->readP = 0;
    fifo->writeP = 0;
    fifo->free = size;
    return;
}

int fifo8_put(struct FIFO *fifo, unsigned char data){
    if(fifo->free <= 0){
        fifo->flags |= FIFO_FLAG_OVERFLOW;
        return -1;
    }
    if(fifo->writeP < fifo->size){
        fifo->writeP++;
    }else{
        fifo->writeP = 0;
    }

    fifo->free--;
    fifo->buf[fifo->writeP] = data;
    return 0;
}

int fifo8_get(struct FIFO *fifo){
    if(fifo->flags & FIFO_FLAG_OVERFLOW
    || fifo->free == fifo->size){
        return -1;
    }

    if(fifo->readP < fifo->size){
        fifo->readP++;
    }else{
        fifo->readP = 0;
    }
    int data = fifo->buf[fifo->readP];
    fifo->free++;

    return data;
}

int fifo8_status(struct FIFO *fifo){
    return fifo->size - fifo->free;
}
