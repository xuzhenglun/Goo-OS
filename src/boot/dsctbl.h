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