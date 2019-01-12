#include "timer.h"
#include "asm.h"
#include "int.h"
#include "memman.h"

struct TIMERCTL timerctl;

void init_pit(void){
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, 0x9c);
    io_out8(PIT_CNT0, 0x2e);
    timerctl.count = 0;

    int i;
    for(i=0; i<MAX_TIMERS; i++){
        timerctl.timers[i].flags = 0;
    }

    return;
}

void inthandler20(int *esp){
    io_out8(PIC0_OCW2, 0x60); // notify PIC that interrupt has been received
    timerctl.count++;

    int i;
    for(i=0; i<MAX_TIMERS; i++){
        if (timerctl.timers[i].flags == TIMER_FLAGS_USING){
            if(timerctl.timers[i].timeout <= timerctl.count){
                timerctl.timers[i].flags = TIMER_FLAGS_ALLOC;
                fifo8_put(timerctl.timers[i].fifo, timerctl.timers[i].data);
            }
        }
    }

    return;
}

struct TIMER *timer_alloc(void){
    int i;
    for(i=0; i < MAX_TIMERS; i++){
        if(timerctl.timers[i].flags == 0){
            timerctl.timers[i].flags = TIMER_FLAGS_ALLOC;
            return &timerctl.timers[i];
        }
    }
    // failed to find an available timer
    return 0;
}

void timer_init(struct TIMER *timer, struct FIFO *fifo, unsigned char data){
    timer->fifo = fifo;
    timer->data = data;
}

void timer_settimeout(struct TIMER *timer, unsigned int timeout){
    timer->timeout = timerctl.count + timeout;
    timer->flags = TIMER_FLAGS_USING;
}


void timer_free(struct TIMER *timer){
    timer->flags = 0;
}
