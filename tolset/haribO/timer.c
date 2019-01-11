#include "timer.h"
#include "asm.h"
#include "int.h"

struct TIMERCTL timerctl;

void init_pit(void){
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, 0x9c);
    io_out8(PIT_CNT0, 0x2e);
    timerctl.count = 0;
    return;
}

void inthandler20(int *esp){
    io_out8(PIC0_OCW2, 0x60); // notify PIC that interrupt has been received
    timerctl.count++;

    if(timerctl.timeout > 0){
        timerctl.timeout--;
        if(timerctl.timeout <= 0){
            fifo8_put(timerctl.fifo, timerctl.data);
        }
    }

    return;
}

void settimer(unsigned int timeout, struct FIFO *fifo, unsigned char data){
    // disable interrupts
    int eflags;
    eflags = io_load_eflags();
    io_cli();

    timerctl.timeout = timeout;
    timerctl.fifo = fifo;
    timerctl.data = data;

    // enable interrupts
    io_store_eflags(eflags);
    return;
}
