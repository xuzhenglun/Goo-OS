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
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	
	char data,s[4];
	io_out8(PIC0_OCW2,0x61);
	data = io_in8(PORT_KEYDAT);
	
	sprintf(s,"%02X",data);	
    boxfill8(binfo->vram, binfo->scrnx, COL8_BLACK, 0, 25, 15, 40);
    print_fonts(binfo->vram, binfo->scrnx, 0, 25, COL8_WHITE, s );
    while(1){
        hlt();
    }
}

void int_handler_2c(int *esp){
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill8(binfo->vram, binfo->scrnx, COL8_BLACK, 0, 0, 32 * 8 - 1, 15);
    print_fonts(binfo->vram, binfo->scrnx, 0, 0, COL8_WHITE,"INT 2c (IRQ-1) :MOUSE");
    while(1){
        hlt();
    }
}
