start:
    MOV     EDX,2
    MOV     EBX,msg
    INT     0x40
    MOV     EDX,4
    INT     0X40
msg:
    DB      "Hello World!",0
