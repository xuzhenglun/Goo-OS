#define    EFLAGS_AC_BIT     0x00040000
#define    CR0_CACHE_DISABLE 0x60000000
#define    MEMMAN_FREES      4090

struct PAGE{
    unsigned int addr,size;
};

struct MEMMAN{
    int frees,maxfrees,lostsize,losts;
    struct PAGE page[1000];
};

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void mem_init(struct MEMMAN *man);
unsigned int mem_total(struct MEMMAN *man);
unsigned int mem_alloc(struct MEMMAN *man, unsigned int size);
int mem_free(struct MEMMAN *man, unsigned int addr,unsigned int size);
