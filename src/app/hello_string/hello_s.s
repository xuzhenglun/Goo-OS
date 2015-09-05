start:
    MOV     EDX,2
    MOV     EBX,msg
    INT     0x40
    RETF

msg:
    DB      "Hello World!",0
