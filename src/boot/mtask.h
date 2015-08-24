#ifndef MTASK_H_H
#define MTASK_H_H

#include "memory.h"
#include "fifo.h"

#define AR_TSS32		0x0089
#define MAX_TASKS       1000
#define TASK_GDT        3
#define TASK_ALLOC      0
#define TASK_SLEEPING   1
#define TASK_RUNNING    2

struct TSS32{
    int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
    int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
    int es,cs,ss,ds,fs,gs;
    int ldtr,iomap;
};

struct TASK{
    int sel,flags,priority;
    struct TSS32 tss;
    struct TASK *child;
    struct TASK *brother;
    struct TASK *father;
    struct FIFO8 kfifo;
};

struct TASKCTRL{
    int running;
    int index;
    struct TASK *now;
    struct TASK *INIT;
    struct TASK *tasks_p[MAX_TASKS];
    struct TASK tasks[MAX_TASKS];
};

struct TASK *task_init(struct MEMMAN *mem);
struct TASK *task_alloc(int priority, int task_addr, int argc, int stack_size);
void task_run(struct TASK *task);
void task_switch();
void task_sleep(struct TASK *task);
void task_wake(struct TASK *task);
void task_kill(struct TASK *task);
void preorder(struct TASK *root,struct TASKCTRL *taskctrl);
void destory_ts(struct TASK *root);
void task_set_priority(struct TASK *task,int priority);
struct TASK *task_now(void);

#endif
