global api_openwin

api_openwin:
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
