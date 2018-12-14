#include "mouse.h"
#include "bootpack.h"
#include "keyboard.h"
#include "asm.h"

void enable_mouse(struct MOUSE_DEC *mdec){
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    // we are waiting for 0xfa from the mouse controller (ACK)
    mdec->phase = 0;
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char data){
    // Read data from mouse fifo buffer, return 1 when entire 3 byte message is read.
    // return 0 in all other cases.
    if(mdec->phase == 0){
        if(data == 0xfa){
            mdec->phase = 1;
        }
        return 0;
    }else if(mdec->phase == 1){
        if((data & 0xc8) != 0x08){return 0;} // first byte of mouse triplet should match this signature
        mdec->buf[0] = data;
        mdec->phase = 2;
        return 0;
    }else if(mdec->phase == 2){
        mdec->buf[1] = data;
        mdec->phase = 3;
        return 0;
    }else if(mdec->phase == 3){
        mdec->buf[2] = data;
        mdec->phase = 1;

        mdec->btn = mdec->buf[0] & 0x07;
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];
        if((mdec->buf[0] & 0x10) != 0){
            mdec->x |= 0xffffff00;
        }
        if((mdec->buf[0] & 0x20) != 0){
            mdec->y |= 0xffffff00;
        }
        mdec->y = -mdec->y;

        return 1;
    }

    // should never reach this PoC, error case.
    return -1;
}
