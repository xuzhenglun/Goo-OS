#include "mtask.h"
#include "timer.h"
#include "dsctbl.h"
#include "memory.h"
#include "basic.h"

struct TASKCTRL *taskctrl;
struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *mem){
    struct TASK *task;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    taskctrl = (struct TASKCTRL *)mem_alloc_4k(mem, sizeof(struct TASKCTRL));
    for(int i = 0; i < MAX_TASKS; i++){
         taskctrl->tasks[i].flags = 0;
         taskctrl->tasks[i].sel   = (TASK_GDT + i) << 3;
         set_segmdesc(gdt + TASK_GDT + i, 103, (int) &taskctrl->tasks[i].tss, AR_TSS32);
    }
    task = task_alloc();
    task->flags = TASK_ACTIVING;
    taskctrl->running = 1;
    taskctrl->now     = 0;
    taskctrl->tasks_p[0] = task;
    load_tr(task->sel);
    task_timer = timer_alloc();
    timer_settime(task_timer, 2);
    return task;
}

struct TASK *task_alloc(void){
     struct TASK *task;
     for(int i = 0; i < MAX_TASKS; i++){
         if(taskctrl->tasks[i].flags == 0){
             task = &taskctrl->tasks[i];
             task->flags = 1;
             task->tss.eflags= 0x00000202;
             task->tss.eax   = 0;
             task->tss.ecx   = 0;
             task->tss.edx   = 0;
             task->tss.ebx   = 0;
             task->tss.ebp   = 0;
             task->tss.esi   = 0;
             task->tss.edi   = 0;
             task->tss.es    = 0;
             task->tss.ds    = 0;
             task->tss.fs    = 0;
             task->tss.gs    = 0;
             task->tss.ldtr  = 0;
             task->tss.iomap = 0x40000000;
             return task;
         }
     }
     return 0;
}

void task_run(struct TASK *task){
    task->flags = 2;
    taskctrl->tasks_p[taskctrl->running] = task;
    taskctrl->running++;
}

void task_switch(void){
     timer_settime(task_timer, 2);
     if(taskctrl->running >= 2){
         taskctrl->now++;
         if(taskctrl->now == taskctrl->running){
              taskctrl->now = 0;
         }
         farjump(0,taskctrl->tasks_p[taskctrl->now]->sel);
     }
}
