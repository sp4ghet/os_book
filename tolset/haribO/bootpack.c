#include <stdio.h>
#include "bootpack.h"
#include "graphics.h"
#include "dsctable.h"
#include "int.h"
#include "fifo.h"
#include "mouse.h"
#include "keyboard.h"

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

    unsigned int mem = memtest(0x00400000, 0xbfffffff) / (1024*1024);
    sprintf(s, "memory: %dMB", mem);
    putfonts8_asci(binfo->vram, binfo->screenX, 0, 64, COL8_ffffff, s);

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

unsigned int memtest(unsigned int start, unsigned int end){
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if((eflg & EFLAGS_AC_BIT) != 0){
        flg486 = 1;
    }

    eflg &= ~EFLAGS_AC_BIT; //bitwise NOT op
    io_store_eflags(eflg);

    if(flg486 != 0){
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if(flg486 != 0){
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    return i;
}

unsigned int memtest_sub(unsigned int start, unsigned int end){
    unsigned int i, old, pat0 = 0xaa55aa5, pat1 = 0x55aa55aa;
    volatile unsigned int *p;
    for(i = start; i < end; i += 0x1000){
        p = (unsigned int *)i;
        old = *p;
        *p = pat0;
        *p ^= 0xffffffff;
        if(*p != pat1){
not_memory:
            *p = old;
            break;
        }
        *p ^= 0xffffffff;
        if(*p != pat0){
            goto not_memory;
        }
        *p = old;
    }
    return i;
}