    MOV     EAX,8
    MOV     DS,AX
    MOV     BYTE [0x102600],0
    MOV     EDX,4
    INT     0x40
