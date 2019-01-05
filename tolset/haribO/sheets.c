#include "sheets.h"
#include "memman.h"

#define SHEET_USE 1

struct SHTCTL *shtctl_init(struct MEMMAN *man, unsigned char* vram, int xsize, int ysize){
    struct SHTCTL *ctl;
    int i;
    ctl = (struct SHTCTL *) memman_alloc_4k(man, sizeof(struct SHTCTL));
    if (ctl == 0){
        return 0;
    }

    ctl->map = (unsigned char *) memman_alloc_4k(man, xsize*ysize);
    if (ctl->map == 0){
        memman_free_4k(man, (int) ctl, sizeof(struct SHTCTL));
        return 0;
    }

    ctl->vram = vram;
    ctl->xsize = xsize;
    ctl->ysize = ysize;
    // no sheets yet
    ctl->top = -1;

    for(i = 0; i < MAX_SHEETS; i++){
        // unused sheet
        ctl->sheets0[i].flags = 0;
        ctl->sheets0[i].ctl = ctl;
    }

    return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl){
    struct SHEET *sht;
    int i;
    for(i=0; i < MAX_SHEETS; i++){
        if (ctl->sheets0[i].flags == 0){
            sht = &ctl->sheets0[i];
            sht->flags = SHEET_USE;
            sht->height = -1;
            return sht;
        }
    }
    // no free sheets
    return 0;
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv){
    sht->buf = buf;
    sht->bxsize = xsize;
    sht->bysize = ysize;
    sht->col_inv = col_inv;
    return;
}

void sheet_map(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
    int h, bx, by, vx, vy, bx0, by0, bx1, by1;
    unsigned char *buf, sid, *map = ctl->map;
    struct SHEET *sht;
    if (vx0 < 0){vx0 = 0;}
    if (vy0 < 0){vy0 = 0;}
    if (vx1 > ctl->xsize){vx1 = ctl->xsize;}
    if (vy1 > ctl->ysize){vy1 = ctl->ysize;}
    for (h = h0; h <= ctl->top; h++)
    {
        sht = ctl->sheets[h];
        sid = sht - ctl->sheets0;
        buf = sht->buf;
        bx0 = vx0 - sht->vx0;
        by0 = vy0 - sht->vy0;
        bx1 = vx1 - sht->vx0;
        by1 = vy1 - sht->vy0;
        if (bx0 < 0)
        {
            bx0 = 0;
        }
        if (by0 < 0)
        {
            by0 = 0;
        }
        if (bx1 > sht->bxsize)
        {
            bx1 = sht->bxsize;
        }
        if (by1 > sht->bysize)
        {
            by1 = sht->bysize;
        }
        for (by = by0; by < by1; by++)
        {
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++)
            {
                vx = sht->vx0 + bx;
                if (buf[by * sht->bxsize + bx] != sht->col_inv)
                {
                    map[vy * ctl->xsize + vx] = sid;
                }
            }
        }
    }
    return;
}

void sheet_updown(struct SHEET *sht, int height){
    int h,
        old = sht->height;

    if (height > sht->ctl->top + 1){
        height = sht->ctl->top + 1;
    }
    if(height < -1){
        height = -1;
    }
    sht->height = height;

    if (old > height){
        if(height >= 0){
            for(h = old; h > height; h--){
                sht->ctl->sheets[h] = sht->ctl->sheets[h-1];
                sht->ctl->sheets[h]->height = h;
            }
            sht->ctl->sheets[height] = sht;
            sheet_map(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
            sheet_refreshsub(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
        }else{
            if(sht->ctl->top > old){
                for(h=old; h < sht->ctl->top; h++){
                    sht->ctl->sheets[h] = sht->ctl->sheets[h + 1];
                    sht->ctl->sheets[h]->height = h;
                }
            }
            sht->ctl->top--;
            sheet_map(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
            sheet_refreshsub(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
        }
    }else if(old < height){
        if(old >= 0){
            for(h = old; h < height; h++){
                sht->ctl->sheets[h] = sht->ctl->sheets[h+1];
                sht->ctl->sheets[h]->height = h;
            }
            sht->ctl->sheets[height] = sht;
        }else{
            for(h=sht->ctl->top; h >= height; h--){
                sht->ctl->sheets[h+1] = sht->ctl->sheets[h];
                sht->ctl->sheets[h+1]->height = h+1;
            }
            sht->ctl->sheets[height] = sht;
            sht->ctl->top++;
        }
        sheet_map(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
        sheet_refreshsub(sht->ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
    }
    return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
    if(sht->height >= 0){
        sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
    }
    return;
}

void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
    int h, bx, by, vx, vy, bx0, by0, bx1, by1;
    unsigned char *buf,
        *vram = ctl->vram,
        *map = ctl->map,
        sid;

    struct SHEET *sht;
    if(vx0 < 0){ vx0 = 0;}
    if(vy0 < 0){ vy0 = 0;}
    if(vx1 > ctl->xsize){ vx1 = ctl->xsize; }
    if(vy1 > ctl->ysize){ vy1 = ctl->ysize; }
    for (h = h0; h <= h1; h++)
    {
        sht = ctl->sheets[h];
        buf = sht->buf;
        sid = sht - ctl->sheets0;

        bx0 = vx0 - sht->vx0;
        by0 = vy0 - sht->vy0;
        bx1 = vx1 - sht->vx0;
        by1 = vy1 - sht->vy0;
        if(bx0 < 0){ bx0 = 0;}
        if(by0 < 0){ by0 = 0;}
        if(bx1 > sht->bxsize){bx1 = sht->bxsize;}
        if(by1 > sht->bysize){by1 = sht->bysize;}

        for (by = by0; by < by1; by++)
        {
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++)
            {
                vx = sht->vx0 + bx;
                if (map[vy*ctl->xsize + vx] == sid)
                {
                    vram[vy*ctl->xsize + vx] = buf[by*sht->bxsize + bx];
                }
            }
        }
    }
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0){
    int oldvx0 = sht->vx0,
        oldvy0 = sht->vy0;
    sht->vx0 = vx0;
    sht->vy0 = vy0;
    if(sht->height >= 0){
        sheet_map(sht->ctl, oldvx0, oldvy0, oldvx0 + sht->bxsize, oldvy0 + sht->bysize, 0);
        sheet_map(sht->ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);

        // previous frame position
        sheet_refreshsub(sht->ctl, oldvx0, oldvy0, oldvx0 + sht->bxsize, oldvy0 + sht->bysize, 0, sht->height - 1);
        // new frame position
        sheet_refreshsub(sht->ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
    }
    return;
}

void sheet_free(struct SHEET *sht){
    if(sht->height >= 0){
        sheet_updown(sht, -1);
    }
    sht->flags = 0;
    return;
}
