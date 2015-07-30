#ifndef INT_H_H
#define INT_H_H
#endif

#include "int.h"

#ifndef BASIC_H_H
#define BASIC_H_H
#endif

#include "basic.h"
#include "../golibc/stdio.h"
#include "bootmain.h"
#include "graph.h"

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

void wait_KBC_sendready(void){
    while(io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY);
}

void init_keyboard(void){
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
}

void enable_mouse(void){
     wait_KBC_sendready();
     io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
     wait_KBC_sendready();
     io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
}
