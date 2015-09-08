#include "console.h"
#include "mtask.h"

int *api_handler(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
    struct CONSOLE *cons = (struct CONSOLE *)*((int *)  0x0fec);
    int ds_base = *((int *)0xfe8);
    struct TASK *task = task_now();
    switch(edx){
        case 1:
            cons_putchar(cons,eax & 0xff);
            break;
        case 2:
            cons_puts(cons,(char *)ebx + ds_base);
            break;
        case 3:
            cons_print(cons,(char *)ebx + ds_base, ecx);
            break;
        case 4:
            return &(task->tss.esp0);
            break;
    }
    return 0;
}
