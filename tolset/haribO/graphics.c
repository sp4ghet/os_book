#include "graphics.h"
#include "bootpack.h"

void boxfill(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    
    int x,y;

    vram += y0*xsize;
    
    for(y=y0; y <= y1; y++){
        for(x=x0; x <= x1; x++){
            vram[x] = c;
        }
        vram += xsize;
    }
}

void init_screen(char* vram, int screenX, int screenY){
    boxfill(vram, screenX, COL8_008484, 0,          0, screenX - 1, screenY - 29);
    boxfill(vram, screenX, COL8_c6c6c6, 0, screenY - 28, screenX - 1, screenY - 28);
    boxfill(vram, screenX, COL8_ffffff, 0, screenY - 27, screenX - 1, screenY - 27);
    boxfill(vram, screenX, COL8_c6c6c6, 0, screenY - 26, screenX - 1, screenY -  1);

    boxfill(vram, screenX, COL8_ffffff,  3, screenY - 24, 59, screenY - 24);
    boxfill(vram, screenX, COL8_ffffff,  2, screenY - 24,  2, screenY - 4);
    boxfill(vram, screenX, COL8_848484,  3, screenY -  4, 59, screenY - 4);
    boxfill(vram, screenX, COL8_848484, 59, screenY - 23, 59, screenY - 5);
    boxfill(vram, screenX, COL8_000000,  2, screenY -  3, 59, screenY - 3);
    boxfill(vram, screenX, COL8_000000, 60, screenY - 24, 60, screenY - 3);


    boxfill(vram, screenX, COL8_848484, screenX - 47, screenY - 24, screenX -  4, screenY - 24);
    boxfill(vram, screenX, COL8_848484, screenX - 47, screenY - 23, screenX - 47, screenY -  4);
    boxfill(vram, screenX, COL8_000000, screenX - 47, screenY -  3, screenX -  4, screenY -  3);
    boxfill(vram, screenX, COL8_000000, screenX -  3, screenY - 24, screenX -  3, screenY -  3);

    return;
}

void init_palette(void){
    static unsigned char table_rgb[16*3] = {
        0x00, 0x00, 0x00, // 0
        0xff, 0x00, 0x00,
        0x00, 0xff, 0x00,
        0xff, 0xff, 0x00,
        0x00, 0x00, 0xff, // 4
        0xff, 0x00, 0xff, 
        0x00, 0xff, 0xff,
        0xff, 0xff, 0xff,
        0xc6, 0xc6, 0xc6,
        0x84, 0x00, 0x00, // 9
        0x00, 0x84, 0x00, 
        0x84, 0x84, 0x00, 
        0x00, 0x00, 0x84,
        0x84, 0x00, 0x84,
        0x00, 0x84, 0x84, // 14
        0x84, 0x84, 0x84 
    };

    set_palette(0, 15, table_rgb);
    return;
}

void set_palette(int start, int end, unsigned char *rgb){
    int i, eflags;
    eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);

    for(i=start; i <= end; i++ ){
        io_out8(0x03c9, rgb[0] /4);
        io_out8(0x03c9, rgb[1] /4);
        io_out8(0x03c9, rgb[2] /4);
        rgb += 3;
    }
    io_store_eflags(eflags);
    return;
}

void putfont8(char* vram, int xsize, int x, int y, char color, char *font){
    char d;
    int i;
    for(i=0; i<16; i++){
        d = font[i];
        char* p = vram + (y+i)*xsize + x; //pointer for this row

        if((d & 0x80) != 0){ p[0] = color; } //leftmost 0b10000000 -> 0x80
        if((d & 0x40) != 0){ p[1] = color; }
        if((d & 0x20) != 0){ p[2] = color; }
        if((d & 0x10) != 0){ p[3] = color; }
        if((d & 0x08) != 0){ p[4] = color; }
        if((d & 0x04) != 0){ p[5] = color; }
        if((d & 0x02) != 0){ p[6] = color; }
        if((d & 0x01) != 0){ p[7] = color; } //rightmost 0b00000001 -> 0x01
    }
    return;
}

void putfonts8_asci(char* vram, int xsize, int x, int y, char color, unsigned char *s){
    extern char hankaku[4096];

    for(; *s != 0x00; s++){
        putfont8(vram, xsize, x, y, color, hankaku + *s * 16);
        x += 8;
    }
    return;
}

void init_cursor(char *mouse, char backgroundColor){
    static char cursor[16][16] = {
        "**************..",
        "*OOOOOOOOOOO*...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
        "..............**"
    };
    int x,y;

    for(y=0; y < 16; y++){
        for(x=0; x<16; x++){
            if(cursor[y][x] == '*'){
                mouse[y*16 + x] = COL8_000000;
            }
            if(cursor[y][x] == 'O'){
                mouse[y*16 + x] = COL8_ffffff;
            }
            if(cursor[y][x] == '.'){
                mouse[y*16 + x] = backgroundColor;
            }
        }
    }

    return;
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize){
    int x,y;
    for(y=0; y < pysize; y++){
        for(x=0; x < pxsize; x++){
            vram[(py0+y)*vxsize + (px0 + x)] = buf[y*bxsize + x];
        }
    }
    return;
}
