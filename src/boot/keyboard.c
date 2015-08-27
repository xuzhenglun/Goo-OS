#include "keyboard.h"
#include "basic.h"

void wait_KBC_sendready(void){
    while(io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY);
}

void init_keyboard(void){
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
}

void enable_mouse(struct MOUSE_DEC *mdec){
     wait_KBC_sendready();
     io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
     wait_KBC_sendready();
     io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    mdec->phase = 0;
}

int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat){
    switch(mdec->phase){
        case 0:
            {
                if (dat == 0xfa)
                    mdec->phase = 1;
                return 0;
            }
        case 1:
            {
                if((dat & 0xc8) == 0x08)
                {
                    mdec->buf[0] = dat;
                    mdec->phase = 2;
                }
                return 0;
            }
        case 2:
            {
                 mdec->buf[1] = dat;
                 mdec->phase = 3;
                 return 0;
            }
        case 3:
            {
                 mdec->buf[2] = dat;
                 mdec->phase = 1;
                 mdec->btn = mdec->buf[0] & 0x07;
                 mdec->x = mdec->buf[1];
                 mdec->y = mdec->buf[2];

                 if((mdec->buf[0] & 0x10) != 0)
                     mdec->x |= 0xffffff00;
                 if((mdec->buf[0] & 0x20) != 0)
                     mdec->y |= 0xffffff00;
                 mdec->y = -mdec->y;
                 return 1;
            }
        default:
                return -1;
    }
}

char keytable0[0x80] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'O', 'P', '[', ']', 0,   0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 0,   0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.' , 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};

char keytable1[0x80] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0,   0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
};

