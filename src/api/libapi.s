global putchar,puts,end_app,mkwindow,win_puts,win_boxfill
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

mkwindow:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBX
    MOV     EDX,5
    MOV     EBX,[ESP+16]
    MOV     ESI,[ESP+20]
    MOV     EDI,[ESP+24]
    MOV     EAX,[ESP+28]
    MOV     ECX,[ESP+32]
    INT     0X40
    POP     EBX
    POP     ESI
    POP     EDI
    RET

win_puts:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBP
    PUSH    EBX
    MOV     EDX,6
    MOV     EBX,[ESP+20]
    MOV     ESI,[ESP+24]
    MOV     EDX,[ESP+28]
    MOV     EAX,[ESP+32]
    MOV     ECX,[ESP+36]
    MOV     EBP,[ESP+40]
    INT     0x40
    POP     EBX
    POP     EBP
    POP     ESI
    POP     EDI
    RET

win_boxfill:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBP
    PUSH    EBX
    MOV     EDX,7
    MOV     EBX,[ESP+20]
    MOV     EAX,[ESP+24]
    MOV     ECX,[ESP+28]
    MOV     ESI,[ESP+32]
    MOV     EDI,[ESP+36]
    MOV     EBP,[ESP+40]
    INT     0x40
    POP     EBX
    POP     EBP
    POP     ESI
    POP     EDI
    RET
