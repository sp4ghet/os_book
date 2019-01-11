#ifndef TIMER_H
#define TIMER_H
#include "fifo.h"

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL {
    unsigned int count;
    unsigned int timeout;
    struct FIFO *fifo;
    unsigned char data;
};

void init_pit(void);
void inthandler20(int *esp);

void settimer(unsigned int timeout, struct FIFO *fifo, unsigned char data);

#endif
