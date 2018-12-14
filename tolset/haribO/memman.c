#include "memman.h"
#include "asm.h"


unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0)
    {
        flg486 = 1;
    }

    eflg &= ~EFLAGS_AC_BIT; //bitwise NOT op
    io_store_eflags(eflg);

    if (flg486 != 0)
    {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if (flg486 != 0)
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    return i;
}

void memman_init(struct MEMMAN *man)
{
    man->frees = 0;
    man->maxfrees = 0;
    man->losts = 0;
    man->lostsize = 0;
}

unsigned int memman_total(struct MEMMAN *man)
{
    unsigned int i, t = 0;
    for (i = 0; i < man->frees; i++)
    {
        t += man->free[i].size;
    }
    return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
    unsigned int i, a;
    for (i = 0; i < man->frees; i++)
    {
        a = man->free[i].addr;
        man->free[i].addr += size;
        man->free[i].size -= size;

        if (man->free[i].size == 0)
        {
            // we used up this sector, so shift the free memory array forward
            man->frees--;
            for (; i < man->frees; i++)
            {
                man->free[i] = man->free[i + 1];
            }
        }
        return a;
    }
    // not enough memory
    return 0;
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i, j;

    // find the sector in the free memory closest to the sector being freed
    for (i = 0; i < man->frees; i++)
    {
        if (man->free[i].addr > addr)
        {
            break;
        }
    }

    if (i > 0)
    {
        if (man->free[i - 1].addr + man->free[i - 1].size == addr)
        {
            //sector being freed is continuous from previous sector
            man->free[i - 1].size += size;
            if (i < man->frees && addr + size == man->free[i].addr)
            {
                //there is a sector in free memory continuous after the sector being freed
                man->free[i - 1].size += man->free[i].size;
                man->frees--;
                for (; i < man->frees; i++)
                {
                    man->free[i] = man->free[i + 1];
                }
            }
            return 0;
        }
    }

    //unable to merge sector being freed with previous sector
    if (i < man->frees && addr + size == man->free[i].addr)
    {
        //able to merge with sector after sector being freed
        man->free[i].addr -= size;
        man->free[i].size += size;
    }

    //unable to merge with anything
    if (man->frees < MEMMAN_FREES)
    {
        for (j = man->frees; j > i; j--)
        {
            // push back all sectors after sector being freed
            man->free[j] = man->free[j - 1];
        }
        man->frees++;
        if (man->maxfrees < man->frees)
        {
            man->maxfrees = man->frees;
        }

        man->free[i].addr = addr;
        man->free[i].size = size;
        return 0;
    }

    man->losts++;
    man->lostsize += size;
    // return failure
    return -1;
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
    unsigned int a;
    size = (size + 0xfff) & 0xfffff000;
    a = memman_alloc(man, size);
    return a;
}

unsigned int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
    unsigned int a;
    size = (size + 0xfff) & 0xfffff000;
    a = memman_free(man, addr, size);
    return a;
}
