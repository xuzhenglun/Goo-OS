global putchar,RETF
extern appmain

putchar:
    MOV     EDX,1
    MOV     AL,[ESP+4]
    INT     0x40
    RET

