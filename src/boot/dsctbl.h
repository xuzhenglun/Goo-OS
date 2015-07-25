#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00000000
#define LIMIT_BOTPAK	0xffffffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

struct SEGMENT_DESCRIPTOR{
    short limit_low;
    short base_low;
    char  base_mid;
    char  access_right;
    char  limit_high;
    char  base_high;
};

struct GATE_DESCRIPTOR{
     short offset_low;
     short selector;
     char  dw_count;
     char  access_right;
     short offset_high;
};


void load_idtr(int limit,int addr);
void load_gdtr(int limit,int addr);

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd,unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);