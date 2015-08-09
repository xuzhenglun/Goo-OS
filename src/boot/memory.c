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
    unsigned int i, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
	volatile unsigned int *p;
    for(i = start; i <= end; i += 0x1000){
         p = (unsigned int *)(i + 0xffc);
         old = *p;
         *p  = pat0;
         *p ^= 0xffffffff;
         if(*p != pat1){
            *p = old;
            break;
         }
         *p ^= 0xffffffff;
         if(*p != pat0){
            *p = old;
            break;
         }
         *p = old;
    }
    return i;
}

void mem_init(struct MEMMAN *man){
     man->frees = 0;
     man->maxfrees = 0;
     man->lostsize = 0;
     man->losts = 0;
}

unsigned int mem_total(struct MEMMAN *man){
     unsigned int t = 0;
     for( int i = 0; i < man->frees; i++ ){
         t += man->page[i].size;
     }
     return t;
}

unsigned int mem_alloc(struct MEMMAN *man, unsigned int size){
    for( unsigned int i = 0; i < man->frees; i++ ){
        if(man->page[i].size >= size){
            unsigned int a = man->page[i].addr;
            man->page[i].addr += size;
            man->page[i].size -= size;
            if(man->page[i].size == 0){
                man->frees --;
                for(; i < man->frees; i++){
                    man->page[i] = man->page[i+1];
                }
            }
            return  a;
        }
    }
	return -1;
}

int mem_free(struct MEMMAN * man,unsigned int addr, unsigned int size){
    int i;
    for(i = 0; i < man->frees; i++){
        if(man->page[i].addr > addr){
            break;
        }
    }

    if(i > 0){
        if(man->page[i-1].addr + man->page[i-1].size == addr){
            man->page[i-1].size += size;
            if(i < man->frees){
                if(addr + size == man->page[i].addr){
                    man->page[i-1].size += man->page[i].size;
                    man->frees --;
                    for(; i < man->frees; i++){
                        man->page[i] = man->page[i+1];
                    }
                }
            }
            return 0;
        }
    }

    if( i < man->frees ){
        if(addr + size == man->page[i].addr){
            man->page[i].addr = addr;
            man->page[i].size += size;
            return 0;
        }
    }

    if(man->frees < MEMMAN_FREES){
        for( int j = man->frees; j > i; j-- ){
            man->page[j] = man->page[j-1];
        }
        man->frees ++;
        if(man->maxfrees < man->frees){
            man->maxfrees = man->frees;
        }
        man->page[i].addr = addr;
        man->page[i].size = size;
        return 0;
    }

    man->losts++;
    man->lostsize += size;
    return -1;
}

unsigned int mem_alloc_4k(struct MEMMAN * man, unsigned int size){
    unsigned int a;
    size = (size + 0xfff) & 0xfffff000;
    a = mem_alloc(man,size);
    return a;
}

int mem_free_4k(struct MEMMAN * man, unsigned int addr, unsigned int size){
    int i;
    size = (size + 0xfff) & 0xfffff000;
    i = mem_free(man,addr,size);
    return i;
}
