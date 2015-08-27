#ifndef KEYBOARD_H_H
#define KEYBOARD_H_H

#define     MOUSECMD_ENABLE     0xf4
#define     KEYCMD_SENDTO_MOUSE 0xd4
#define     KEYCMD_LED          0xed
#define     PORT_KEYDAT         0x0060
#define     PORT_KEYSTA         0x0064
#define     PORT_KEYCMD         0x0064
#define     KEYSTA_SEND_NOTREADY 0x02
#define     KEYCMD_WRITE_MODE   0x60
#define     KBC_MODE            0x47

struct MOUSE_DEC{
    unsigned char buf[3],phase;
    int x,y,btn;
};

void wait_KBC_sendready(void);
void enable_mouse(struct MOUSE_DEC *mdec);
int  mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat);
void wait_KBC_sendready(void);
void init_keyboard(void);

#endif
