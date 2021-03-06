#include "console.h"
#include "graph.h"
#include "mtask.h"

int *api_handler(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
    struct CONSOLE *cons = (struct CONSOLE *)*((int *)  0x0fec);
    int ds_base = *((int *)0xfe8);
    struct TASK *task = task_now();
    struct LAYER_CTL *layctl = (struct LAYER_CTL *)*((int *) 0x0fe4);
    struct LAYER *lay;
    int *reg = &eax + 1;
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
        case 5:
            lay = layer_alloc(layctl);
            layer_setbuf(lay, (unsigned char *)ebx + ds_base, esi, edi, eax);
            make_window8((unsigned char *)ebx + ds_base, esi, edi, (char *)ecx + ds_base, 0);
            layer_slide(lay, 100, 50);
            layer_updown(lay, 3);
            reg[7] = (int)lay;
            break;
        case 6:
            lay = (struct LAYER *)ebx;
            print_fonts(lay->buf, lay->bxsize, esi, edi, eax, (char *)ebp + ds_base);
            layer_refresh(lay, esi, edi, esi + (ecx << 3), edi + 16);
            break;
        case 7:
            lay = (struct LAYER *)ebx;
            boxfill8(lay->buf, lay->bxsize, ebp, eax, ecx, esi ,edi);
            layer_refresh(lay, eax, ecx, esi + 1, edi + 1);
    }
    return 0;
}
