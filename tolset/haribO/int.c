#include "int.h"
#include "graphics.h"
#include "bootpack.h"

struct KEYBUF keybuf;

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

    if(keybuf.flag == 0){
        keybuf.data = data;
        keybuf.flag = 1;
    }
   
    return;
}

void inthandler2c(int *esp){
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill(binfo->vram, binfo->screenX, COL8_000000,  0, 0, 32*8-1, 15);
    putfonts8_asci(binfo->vram, binfo->screenX, 0, 0, COL8_ffffff, "INT 2C (IRQ-12) : PS/2 Mouse");
    for(;;){
        io_hlt();
    }
}

void inthandler27(int *esp){
    io_out8(PIC0_OCW2, 0x67);
    return;
}
