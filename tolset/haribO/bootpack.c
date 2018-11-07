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

    init_gdtidt();
    init_pic();
    io_sti();

    unsigned char *kbuf;
    fifo8_init(&keybuf, 32, kbuf);
    unsigned char *mbuf;
    fifo8_init(&mousebuf, 128, mbuf);
    
    unsigned char mouse_dbuf[3], mouse_phase;

    init_palette();
    init_screen(binfo->vram, binfo->screenX, binfo->screenY);
    init_keyboard();
    enable_mouse();
    mouse_phase = 0;

    char s[40], mcursor[256];
    int mx = (binfo->screenX - 16) / 2,
        my = (binfo->screenY - 28 - 16) / 2;
    init_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->screenX, 16, 16, mx, my, mcursor, 16);
    sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asci(binfo->vram, binfo->screenX, 0, 0, COL8_ffffff, s);

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
                if(mouse_phase == 0){
                    if (i == 0xfa){
                        mouse_phase = 1;
                    }
                }else if(mouse_phase == 1){
                   mouse_dbuf[0] = i;
                   mouse_phase = 2;
                }else if(mouse_phase == 2){
                    mouse_dbuf[1] = i;
                    mouse_phase = 3;
                }else if(mouse_phase == 3){
                    mouse_dbuf[2] = i;
                    sprintf(s, "%02X %02X %02X ", mouse_dbuf[0], mouse_dbuf[1], mouse_dbuf[2]);
                    boxfill(binfo->vram, binfo->screenX, COL8_008484, 32, 16, 32 + 8*8-1, 31);
                    putfonts8_asci(binfo->vram, binfo->screenX, 32, 16, COL8_ffffff, s);
                    mouse_phase = 1;
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

void enable_mouse(void){
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    return;
}