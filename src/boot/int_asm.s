    EXTERN int_handler_21,int_handler_2c
    global asm_int_handler_21,asm_int_handler_2c,load_gdtr,load_idtr

load_gdtr:		; void load_gdtr(int limit, int addr);
        MOV		AX,[ESP+4]		; limit
        MOV		[ESP+6],AX
        LGDT	[ESP+6]
        RET

load_idtr:		; void load_idtr(int limit, int addr);
        MOV		AX,[ESP+4]		; limit
        MOV		[ESP+6],AX
        LIDT	[ESP+6]
        RET

asm_int_handler_21:

    PUSH ES
    PUSH DS
    PUSHAD
    MOV  EAX,ESP
    PUSH EAX
    MOV  AX,SS
    MOV  DS,AX
    MOV  ES,AX
    CALL int_handler_21
    POP  EAX
    POPAD
    POP  DS
    POP  ES
    IRETD


asm_int_handler_2c:

    PUSH ES
    PUSH DS
    PUSHAD
    MOV  EAX,ESP
    PUSH EAX
    MOV  AX,SS
    MOV  DS,AX
    MOV  ES,AX
    CALL int_handler_2c
    POP  EAX
    POPAD
    POP  DS
    POP  ES
    IRETD
