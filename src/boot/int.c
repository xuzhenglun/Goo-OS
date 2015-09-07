#include "int.h"
#include "basic.h"
#include "../golibc/stdio.h"
#include "bootmain.h"
#include "graph.h"
#include "timer.h"
#include "mtask.h"
#include "console.h"
#include "../api/libapi.h"

void init_pic(void){
    io_out8(PIC0_IMR,  0xff  );
    io_out8(PIC1_IMR,  0xff  );

    io_out8(PIC0_ICW1, 0x11  );
    io_out8(PIC0_ICW2, 0x20  );
    io_out8(PIC0_ICW3, 1 << 2);
    io_out8(PIC0_ICW4, 0x01  );

    io_out8(PIC1_ICW1, 0x11  );
    io_out8(PIC1_ICW2, 0x28  );
    io_out8(PIC1_ICW3, 2     );
    io_out8(PIC1_ICW4, 0x01  );

    io_out8(PIC0_IMR,  0xfb  );
    io_out8(PIC1_IMR,  0xff  );
}

#define PORT_KEYDAT		0x0060

void int_handler_20(int *esp){
    extern struct TIMERCTRL timerctrl;
    extern struct TIMER *task_timer;
    char ts_flag = 0;
    io_out8(PIC0_OCW2, 0x60);
    timerctrl.count++;
    if(timerctrl.next > timerctrl.count){
        return;
    }
    else{
         while(timerctrl.head->timeout <= timerctrl.count){
            timerctrl.head->flags = TIMER_FLAGS_ALLOC;
            if(timerctrl.head != task_timer)
                fifo8_put(timerctrl.head->fifo, timerctrl.head->data);
            else{
                ts_flag = 1;
            }
            timerctrl.head = timerctrl.head->next;
         }
    }
    timerctrl.next = timerctrl.head->timeout;

    if(ts_flag != 0){
        task_switch();
    }
}

void int_handler_21(int *esp){
	io_out8(PIC0_OCW2,0x61);
	unsigned char data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo,data);
}

void int_handler_27(int *esp)
/* PIC0からの不完全割り込み対策 */
/* Athlon64X2機などではチップセットの都合によりPICの初期化時にこの割り込みが1度だけおこる */
/* この割り込み処理関数は、その割り込みに対して何もしないでやり過ごす */
/* なぜ何もしなくていいの？
	→  この割り込みはPIC初期化時の電気的なノイズによって発生したものなので、
		まじめに何か処理してやる必要がない。									*/
{
	io_out8(PIC0_OCW2, 0x67); /* IRQ-07受付完了をPICに通知 */
	return;
}

void int_handler_2c(int *esp){
    unsigned char data;
    io_out8(PIC1_OCW2, 0x64);
    io_out8(PIC0_OCW2, 0x62);
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&mousefifo, data);
}

int *int_handler_0d(int *esp){
    struct CONSOLE *cons = (struct CONSOLE *)*((int *)0x0fec);
    struct TASK *task = task_now();
    /*putchar('!');*/
    cons_puts(cons,"\nInt 0x0D :\n\tGeneral Protection Exception\n");
    return &(task->tss.esp0);
}
