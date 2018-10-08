void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
extern char hankaku[4096];

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

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void boxfill(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* vram, int screenX, int screenY);
void putfont8(char* vram, int xsize, int x, int y, char c, char *font);

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};

void HariMain(void){
    
    init_palette();

    struct BOOTINFO *binfo = 0x00ff0;

    int xsize = binfo->screenX;
    int ysize = binfo->screenY;
    char* vram = binfo->vram;
    
    init_screen(vram, xsize, ysize);

    putfont8(vram, xsize, 8, 8, COL8_ffffff, hankaku + 'A'*16);

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

