global putchar,puts,end_app
extern appmain

putchar:
    MOV     EDX,1
    MOV     AL,[ESP+4]
    INT     0x40
    RET

puts:
    PUSH    EBX
    MOV     EDX,2
    MOV     EBX,[ESP+8]
    INT     0x40
    POP     EBX
    RET

end_app:
    MOV     EDX,4
    INT     0x40
