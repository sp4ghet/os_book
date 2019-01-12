#ifndef TIMER_H
#define TIMER_H
#include "fifo.h"

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

#define MAX_TIMERS 500

struct TIMER{
    unsigned int timeout, flags;
    struct FIFO *fifo;
    unsigned char data;
};

struct TIMERCTL {
    unsigned int count;
    struct TIMER timers[MAX_TIMERS];
};


void init_pit(void);
void inthandler20(int *esp);

struct TIMER *timer_alloc(void);
void timer_init(struct TIMER *timer, struct FIFO *fifo, unsigned char data);
void timer_settimeout(struct TIMER *timer, unsigned int timeout);
void timer_free(struct TIMER *timer);

#endif
