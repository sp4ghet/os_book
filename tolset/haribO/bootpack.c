#include <stdio.h>
#include "asm.h"
#include "bootpack.h"
#include "graphics.h"
#include "dsctable.h"
#include "int.h"
#include "fifo.h"
#include "mouse.h"
#include "keyboard.h"
#include "memman.h"
#include "sheets.h"

extern struct FIFO keybuf;
extern struct FIFO mousebuf;

void HariMain(void){
    struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;
    struct MOUSE_DEC mdec;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct SHTCTL *shtctl;
    struct SHEET *sheet_back, *sheet_mouse;
    unsigned char *buf_back, buf_mouse[256];

    unsigned char *kbuf, *mbuf;
    char s[40];
    int mx = (binfo->screenX - 16) / 2,
        my = (binfo->screenY - 28 - 16) / 2;

    // initialize GDT/IDT and PIC
    // we do this first because the asmhead doesn't load the proper GDT/IDT and we will have garbage interrupts piling up in the PIC
    init_gdtidt();
    init_pic();
    io_sti();

    fifo8_init(&keybuf, 32, kbuf);
    fifo8_init(&mousebuf, 128, mbuf);

    unsigned int mem_total = memtest(0x00400000, 0xbfffffff);
    memman_init(memman);
    memman_free(memman, 0x00001000, 0x0009e000);
    memman_free(memman, 0x00400000, mem_total - 0x00400000);

    // initialize various modules
    init_palette();
    shtctl =  shctl_init(memman, binfo->vram, binfo->screenX, binfo->screenY);
    sheet_back = sheet_alloc(shtctl);
    sheet_mouse = sheet_alloc(shtctl);
    buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->screenX * binfo->screenY);
    sheet_setbuf(sheet_back, buf_back, binfo->screenX, binfo->screenY, -1); // no transparent color
    sheet_setbuf(sheet_mouse, buf_mouse, 16, 16, 99); // color 99 is transparent
    init_screen(buf_back, binfo->screenX, binfo->screenY);
    init_cursor(buf_mouse, 99);
    sheet_slide(shtctl, sheet_back, 0, 0);
    sheet_slide(shtctl, sheet_mouse, mx, my);
    sheet_updown(shtctl, sheet_back, 0);
    sheet_updown(shtctl, sheet_mouse, 1);
    sprintf(s, "(%d, %d)", mx, my);
    boxfill(buf_back, sheet_back->bxsize, COL8_008484, 0, 0, 160, 16);
    putfonts8_asci(buf_back, sheet_back->bxsize, 0, 0, COL8_ffffff, s);
    sprintf(s, "memory: %dMB, free: %dMB", mem_total / (1024*1024), memman_total(memman) / (1024*1024));
    putfonts8_asci(buf_back, binfo->screenX, 0, 64, COL8_ffffff, s);
    sheet_refresh(shtctl, sheet_back, 0, 0, sheet_back->bxsize, sheet_back->bysize);

    init_keyboard();
    enable_mouse(&mdec);

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
                boxfill(buf_back, binfo->screenX, COL8_008484, 0, 16, 15, 31);
                putfonts8_asci(buf_back, binfo->screenX, 0, 16, COL8_ffffff, s);
                sheet_refresh(shtctl, sheet_back, 0, 16, 16, 32);
            }else if(fifo8_status(&mousebuf) != 0){
                int i = fifo8_get(&mousebuf);
                io_sti();
                if(mouse_decode(&mdec, i) == 1){
                    sprintf(s, "%02X %02X %02X ", mdec.buf[0], mdec.buf[1], mdec.buf[2]);
                    boxfill(buf_back, binfo->screenX, COL8_008484, 32, 16, 32 + 8*8-1, 31);
                    putfonts8_asci(buf_back, binfo->screenX, 32, 16, COL8_ffffff, s);

                    mx += mdec.x;
                    my += mdec.y;
                    mx = mx < 0
                        ? 0
                        : mx >= binfo->screenX - 16
                            ? binfo->screenX - 16
                            : mx;
                    my = my < 0
                             ? 0
                             : my >= binfo->screenY - 16
                                   ? binfo->screenY - 16
                                   : my;
                    sheet_slide(shtctl, sheet_mouse, mx, my);
                    sprintf(s, "(%d, %d) click: %1x", mx, my, mdec.btn);
                    boxfill(buf_back, binfo->screenX, COL8_008484, 0, 0, 160, 16);
                    putfonts8_asci(buf_back, binfo->screenX, 0, 0, COL8_ffffff, s);
                    sheet_refresh(shtctl, sheet_back, 0, 0, binfo->screenX, 32);
                }
            }
        }
    }
}
