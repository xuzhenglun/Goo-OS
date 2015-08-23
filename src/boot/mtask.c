#include "mtask.h"
#include "timer.h"
#include "dsctbl.h"
#include "memory.h"
#include "basic.h"

struct TASKCTRL *taskctrl;
struct TIMER *task_timer;
extern struct MEMMAN *memman; //内存管理块的内存位置

struct TASK *task_init(struct MEMMAN *mem){
    struct TASK *task;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    taskctrl = (struct TASKCTRL *)mem_alloc_4k(mem, sizeof(struct TASKCTRL));
    for(int i = 0; i < MAX_TASKS; i++){
         taskctrl->tasks[i].flags = TASK_ALLOC;
         taskctrl->tasks[i].sel   = (TASK_GDT + i) << 3;
         set_segmdesc(gdt + TASK_GDT + i, 103, (int) &taskctrl->tasks[i].tss, AR_TSS32);
    }
    task = task_alloc(1,0x0,0,0);
    task->flags = TASK_RUNNING;
    task->father = -1;//INIT 任务没有父进程，与初始化进程的父进程为0作区别，取-1
    taskctrl->running = 1;
    taskctrl->now     = task;
    taskctrl->INIT = task;
    load_tr(task->sel);
    task_timer = timer_alloc();
    timer_settime(task_timer, 2);
    return task;
}

struct TASK *task_alloc(int priority, int task_addr, int argc, int stack_size){
     struct TASK *task;
     for(int i = 0; i < MAX_TASKS; i++){
         if(taskctrl->tasks[i].flags == 0){
             task = &taskctrl->tasks[i];
             task->flags = TASK_SLEEPING;
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
             task->priority  = priority;
             task->tss.esp   = mem_alloc_4k(memman, stack_size *1024) + stack_size * 1024 - argc * 4 - 8;
             task->tss.eip   = task_addr;
             task->tss.es    = 1 << 3;
             task->tss.cs    = 2 << 3;
             task->tss.ss    = 1 << 3;
             task->tss.ds    = 1 << 3;
             task->tss.fs    = 1 << 3;
             task->tss.gs    = 1 << 3;
             task->father    = 0;
             return task;
         }
     }
     return 0;
}

void task_run(struct TASK *task){
    task->flags = TASK_RUNNING;

    if(taskctrl->now->child != 0){
        struct TASK *tmp = taskctrl->now->child;
        while(tmp->brother != 0){
            tmp = tmp->brother;
        }
        tmp->brother = task;
    }else{
        taskctrl->now->child = task;
    }
    task->father = taskctrl->now;

    preorder(taskctrl->INIT, taskctrl);
}

void task_switch(void){
     timer_settime(task_timer, taskctrl->now->priority);
     if(taskctrl->running >= 2){
         taskctrl->index++;
         if(taskctrl->index >= taskctrl->running){
             taskctrl->index = 0;
         }
         if(taskctrl->now != taskctrl->tasks_p[taskctrl->index]){
             taskctrl->now = taskctrl->tasks_p[taskctrl->index];
             farjump(0,taskctrl->tasks_p[taskctrl->index]->sel);}
     }
}

void preorder(struct TASK *root,struct TASKCTRL *taskctrl){
    if(root == taskctrl->INIT){
        taskctrl->running = 0;
    }
    if(root != 0){
        preorder(root->brother,taskctrl);
        if(root->flags != TASK_RUNNING){
            return;
        }
        taskctrl->tasks_p[taskctrl->running++] = root;
        preorder(root->child,  taskctrl);
    }
}

void task_sleep(struct TASK *task){
    if(task->father == -1 || taskctrl->running == 1){
        hlt();
    }else{
        task->flags = TASK_SLEEPING;
        preorder(taskctrl->INIT, taskctrl);
        timer_settime(task_timer, task->father->priority);
        taskctrl->now = task->father;
        farjump(0,task->father->sel);
    }
}

void task_kill(struct TASK *task){
    int ret = task->father->sel;
    destory_ts(task);
    preorder(taskctrl->INIT, taskctrl);
    timer_settime(task_timer, task->father->priority);
    farjump(0,ret);
}

void destory_ts(struct TASK *root){
    if(root != 0){
        destory_ts(root->brother);
        destory_ts(root->child);
        root->flags = TASK_ALLOC;
    }
}

void task_wake(struct TASK *task){
    if(taskctrl->running == 1 && task->flags == TASK_RUNNING){
        return;
    }else{
        task->flags = TASK_RUNNING;
        preorder(taskctrl->INIT, taskctrl);
    }
}

void task_set_priority(struct TASK *task,int priority){
    task->priority = priority;
}

struct TASK *task_now(void){
    return taskctrl->now;
}
