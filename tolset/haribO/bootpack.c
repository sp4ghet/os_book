#include <stdio.h>
#include "bootpack.h"
#include "graphics.h"
#include "dsctable.h"
#include "int.h"

void HariMain(void){
    struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;

    init_gdtidt();
    init_pic();
    io_sti();
    
    init_palette();
    init_screen(binfo->vram, binfo->screenX, binfo->screenY);
    // putfont8(vram, xsize, 8, 8, COL8_ffffff, hankaku + 'A'*16);
    // putfonts8_asci(binfo->vram, binfo->screenX, 8, 8, COL8_ffffff, "ABC 123");
    // putfonts8_asci(binfo->vram, binfo->screenX, 31, 31, COL8_000000, "Haribote os.");
    // putfonts8_asci(binfo->vram, binfo->screenX, 30, 30, COL8_ffffff, "Haribote os.");
    // char* s = "";
    // sprintf(s, "cylinders = 0x%05x", binfo->cylinders);
    // putfonts8_asci(binfo->vram, binfo->screenX, 30, 60, COL8_ffffff, s);

    char* s[40], mcursor[256];
    int mx = (binfo->screenX - 16) / 2,
        my = (binfo->screenY - 28 - 16) / 2;
    init_cursor(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->screenX, 16, 16, mx, my, mcursor, 16);
    sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asci(binfo->vram, binfo->screenX, 0, 0, COL8_ffffff, s);

    io_out8(PIC0_IMR, 0xf9);
    io_out8(PIC1_IMR, 0xef);

    for(;;){
        io_hlt();
    }

}
