#include <stdio.h>
#include "bootpack.h"
#include "graphics.h"
#include "dsctable.h"
#include "int.h"
#include "fifo.h"

extern struct FIFO keybuf;
extern struct FIFO mousebuf;

void HariMain(void){
    struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;
    struct MOUSE_DEC mdec; 
    unsigned char *kbuf, *mbuf;
    char s[40], mcursor[16*16];
    int mx = (binfo->screenX - 16) / 2,
        my = (binfo->screenY - 28 - 16) / 2;

    // initialize GDT/IDT and PIC
    // we do this first because the asmhead doesn't load the proper GDT/IDT and we will have garbage interrupts piling up in the PIC
    init_gdtidt();
    init_pic();
    io_sti();

    fifo8_init(&keybuf, 32, kbuf);
    fifo8_init(&mousebuf, 128, mbuf);

    // initialize various modules
    init_palette();
    init_screen(binfo->vram, binfo->screenX, binfo->screenY);
    init_keyboard();
    enable_mouse(&mdec);

    // initialize mouse cursor and draw on screen
    init_cursor(mcursor, COL8_008484);
   
    // initialize mouse and keyboard interrupt handlers
    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    for(;;){
        io_cli();
        if(fifo8_status(&keybuf) + fifo8_status(&mousebuf) == 0){
            io_stihlt();
        }else{
            if(fifo8_status(&keybuf) != 0){
                int i = fifo8_get(&keybuf);
                io_sti();
                sprintf(s, "%02X", i);
                boxfill(binfo->vram, binfo->screenX, COL8_008484, 0, 16, 15, 31);
                putfonts8_asci(binfo->vram, binfo->screenX, 0, 16, COL8_ffffff, s);   
            }else if(fifo8_status(&mousebuf) != 0){
                int i = fifo8_get(&mousebuf);
                io_sti();
                if(mouse_decode(&mdec, i) == 1){
                    sprintf(s, "%02X %02X %02X ", mdec.buf[0], mdec.buf[1], mdec.buf[2]);
                    boxfill(binfo->vram, binfo->screenX, COL8_008484, 32, 16, 32 + 8*8-1, 31);
                    putfonts8_asci(binfo->vram, binfo->screenX, 32, 16, COL8_ffffff, s);


                    boxfill(binfo->vram, binfo->screenX, COL8_008484, mx, my, mx+16, my+16);
                    mx += mdec.x;
                    my += mdec.y;
                    putblock8_8(binfo->vram, binfo->screenX, 16, 16, mx, my, mcursor, 16);
                    sprintf(s, "(%d, %d) click: %1x", mdec.x, mdec.y, mdec.btn);
                    boxfill(binfo->vram, binfo->screenX, COL8_008484, 0, 0, 8*(strlen(s) + 3), 16);
                    putfonts8_asci(binfo->vram, binfo->screenX, 0, 0, COL8_ffffff, s);
                }
            }
        }
    }
}

void wait_KBC_sendready(void){
    for(;;){
        if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0){
            break;
        }
    }
    return;
}

void init_keyboard(void){
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

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