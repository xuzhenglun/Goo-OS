    EXTERN int_handler_21,int_handler_2c,int_handler_27,int_handler_20,api_handler
    global asm_int_handler_21,asm_int_handler_2c,asm_int_handler_27,asm_int_handler_20,asm_api_handler

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
	
asm_int_handler_27:

    PUSH ES
    PUSH DS
    PUSHAD
    MOV  EAX,ESP
    PUSH EAX
    MOV  AX,SS
    MOV  DS,AX
    MOV  ES,AX
    CALL int_handler_27
    POP  EAX
    POPAD
    POP  DS
    POP  ES
    IRETD

asm_int_handler_20:

    PUSH ES
    PUSH DS
    PUSHAD
    MOV  EAX,ESP
    PUSH EAX
    MOV  AX,SS
    MOV  DS,AX
    MOV  ES,AX
    CALL int_handler_20
    POP  EAX
    POPAD
    POP  DS
    POP  ES
    IRETD

asm_api_handler:
    STI
    PUSHAD
    PUSHAD ;传值
    CALL    api_handler
    ADD     ESP,32
    POPAD
    IRETD
