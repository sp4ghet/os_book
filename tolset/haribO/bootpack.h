#ifndef BOOTPACK_H
#define BOOTPACK_H

#define ADR_BOOTINFO 0x00ff0

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

#define MEMMAN_FREES 4090

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};

struct FREEINFO {
    unsigned int addr, size;
};

struct MEMMAN {
    int frees, maxfrees, lostsize, losts;
    struct FREEINFO free[MEMMAN_FREES]
};

// functions defined in naskfunc.nas in assembly
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_out8(int port, int data);
int io_in8(int port);
int io_load_eflags(void);
void io_store_eflags(int eflags);
unsigned int load_cr0();
void store_cr0(unsigned int cr0);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);

// functions in bootpack.c
void HariMain(void);
unsigned int memtest(unsigned int start, unsigned int end);

void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man);

#endif
