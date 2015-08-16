#ifndef MTASK_H_H
#define MTASK_H_H

#include "memory.h"

#define AR_TSS32		0x0089
#define MAX_TASKS       1000
#define TASK_GDT        3
#define TASK_ACTIVING   2

struct TSS32{
    int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
    int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
    int es,cs,ss,ds,fs,gs;
    int ldtr,iomap;
};

struct TASK{
    int sel,flags;
    struct TSS32 tss;
};

struct TASKCTRL{
    int running;
    int now;
    struct TASK *tasks_p[MAX_TASKS];
    struct TASK tasks[MAX_TASKS];
};

struct TASK *task_init(struct MEMMAN *mem);
struct TASK *task_alloc(void);
void task_run();
void task_switch();
void task_sleep();
void task_kill();

#endif
