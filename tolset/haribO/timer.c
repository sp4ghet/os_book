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
    return;
}
