#ifndef TIMER_H
#define TIMER_H

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL {
    unsigned int count;
};

void init_pit(void);
void inthandler20(int *esp);

#endif
