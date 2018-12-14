#include "int.h"
#include "graphics.h"
#include "bootpack.h"
#include "fifo.h"
#include "asm.h"

struct FIFO keybuf;
struct FIFO mousebuf;

void init_pic(void){
    // disable all interrupts
    io_out8(PIC0_IMR, 0xff);
    io_out8(PIC1_IMR, 0xff);

    io_out8(PIC0_ICW1, 0x11);
    io_out8(PIC0_ICW2, 0x20);
    io_out8(PIC0_ICW3, 1 << 2);
    io_out8(PIC0_ICW4, 0x01);

    io_out8(PIC1_ICW1, 0x11);
    io_out8(PIC1_ICW2, 0x28);
    io_out8(PIC1_ICW3, 2);
    io_out8(PIC1_ICW4, 0x01);

    io_out8(PIC0_IMR, 0xfb); // disable all but PIC1
    io_out8(PIC1_IMR, 0xff); // disable all interrupts
}

// keyboard
void inthandler21(int *esp){
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61); //Notify PIC that IRQ-01 is read
    data = io_in8(PORT_KEYDAT);

    fifo8_put(&keybuf, data);

    return;
}

void inthandler2c(int *esp){
    unsigned char data;
    io_out8(PIC1_OCW2, 0x64);
    io_out8(PIC0_OCW2, 0x62);

    data = io_in8(PORT_KEYDAT);

    fifo8_put(&mousebuf, data);
    return;
}

void inthandler27(int *esp){
    io_out8(PIC0_OCW2, 0x67);
    return;
}
