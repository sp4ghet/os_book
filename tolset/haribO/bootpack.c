#include <stdio.h>

#pragma region assembly defined
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

#pragma endregion


#pragma region macros

#define COL8_000000 0
#define COL8_ff0000 1
#define COL8_00ff00 2
#define COL8_ffff00 3
#define COL8_0000ff 4
#define COL8_ff00ff 5
#define COL8_00ffff 6
#define COL8_ffffff 7
#define COL8_c6c6c6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

#pragma endregion

#pragma region structs

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};

struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

#pragma endregion

#pragma region function declarations

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* vram, int screenX, int screenY);

void putfont8(char* vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asci(char* vram, int xsize, int x, int y, char color, unsigned char *s);

void init_cursor(char *mouse, char color);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#pragma endregion


void HariMain(void){
    
    init_palette();

    struct BOOTINFO *binfo = (struct BOOTINFO*) 0x00ff0;

    int xsize = binfo->screenX;
    int ysize = binfo->screenY;
    char* vram = binfo->vram;
    
    init_gdtidt();
    init_screen(vram, xsize, ysize);

    // putfont8(vram, xsize, 8, 8, COL8_ffffff, hankaku + 'A'*16);
    putfonts8_asci(vram, xsize, 8, 8, COL8_ffffff, "ABC 123");
    putfonts8_asci(vram, xsize, 31, 31, COL8_000000, "Haribote os.");
    putfonts8_asci(vram, xsize, 30, 30, COL8_ffffff, "Haribote os.");

    char* s = "";
    sprintf(s, "cylinders = 0x%05x", binfo->cylinders);
    putfonts8_asci(vram, xsize, 30, 60, COL8_ffffff, s);

    char* mcursor = "";
    int mx = 160,
        my = 100;
    init_cursor(mcursor, COL8_008484);
    putblock8_8(vram, xsize, 16, 16, mx, my, mcursor, 16);

    for(;;){
        io_hlt();
    }

}


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

void init_gdtidt(void){
    struct SEGMENT_DESCRIPTOR   *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
    struct GATE_DESCRIPTOR      *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;

    // setup Global (segment) Descriptor Table
    int i;
    for(i=0; i < 8192; i++){
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt+1, 0xffffffff, 0x00000000, 0x4092);
    set_segmdesc(gdt+2, 0x0007ffff, 0x00280000, 0x409a);
    load_gdtr(0xffff, 0x00270000);


    // setup Interrupt Descriptor Table
    for(i=0; i < 256; i++){
        set_gatedesc(idt+i, 0,0,0);
    }
    load_idtr(0x7ff, 0x0026f800);

    return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
    if(limit > 0xfffff){
     ar |= 0x8000;
     limit /= 0x1000;   
    }

    sd->limit_low       = limit & 0xffff;
    sd->base_low        = base & 0xffff;
    sd->base_mid        = (base >> 16) & 0xff;
    sd->access_right    = ar & 0xff;
    sd-> limit_high     = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high       = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
    gd->offset_low      = offset & 0xffff;
    gd->selector        = selector;
    gd->dw_count        = (ar >> 8) & 0xff;
    gd->access_right    = ar & 0xff;
    gd->offset_high     = (offset >> 16) & 0xffff;
    return;
}