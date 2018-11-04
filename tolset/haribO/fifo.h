#ifndef FIFO_H
#define FIFO_H

#define FIFO_FLAG_OVERFLOW 0x0001

struct FIFO {
    unsigned char *buf;
    int readP, writeP, size, free, flags;
};

void fifo8_init(struct FIFO *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO *fifo, unsigned char data);
int fifo8_get(struct FIFO *fifo);
int fifo8_status(struct FIFO *fifo);

#endif