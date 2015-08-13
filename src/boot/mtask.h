#ifndef MTASK_H_H
#define MTASK_H_H

#define AR_TSS32		0x0089

struct TSS32{
    int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
    int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
    int es,cs,ss,ds,fs,gs;
    int ldtr,iomap;
};


#endif