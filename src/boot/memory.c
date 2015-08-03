#include "memory.h"
#include "basic.h"

unsigned int memtest(unsigned int start, unsigned int end){
    char flag486 = 0;
    unsigned int eflg,cr0,i;

    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if((eflg & EFLAGS_AC_BIT) != 0){
        flag486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    if(flag486 != 0){
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    i = memtest_sub(start,end);

    if(flag486 != 0){
         cr0 = load_cr0();
         cr0 &= ~CR0_CACHE_DISABLE;
         store_cr0(cr0);
    }

    return i;
}

unsigned int memtest_sub(unsigned int start, unsigned int end){
    volatile unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    for(i = start; i <= end; i += 0x1000){
         p = (unsigned int *)(i + 0xffc);
         old = *p;
         *p  = pat0;
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
