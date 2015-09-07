    EXTERN int_handler_21,int_handler_2c,int_handler_27,int_handler_20,int_handler_0c,int_handler_0d,api_handler
    global asm_int_handler_21,asm_int_handler_2c,asm_int_handler_27,asm_int_handler_20,asm_int_handler_0c,asm_api_handler,asm_int_handler_0d,start_app

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

asm_int_handler_0d:
    STI
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    int_handler_0d
    CMP     EAX,0
    JNE     end_app
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    ADD     ESP,4
    IRETD

asm_int_handler_0c:
    STI
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    int_handler_0c
    CMP     EAX,0
    JNE     end_app
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    ADD     ESP,4
    IRETD

asm_api_handler:
    STI
    PUSH    DS
    PUSH    ES
    PUSHAD
    PUSHAD ;传值
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    api_handler
    CMP     EAX,0
    JNE     end_app
    ADD     ESP,32
    POPAD
    POP     ES
    POP     DS
    IRETD

end_app:
    MOV     ESP,[EAX]
    MOV     DWORD [EAX+4],0
    POPAD
    RET

start_app:
    PUSHAD
    MOV     EAX,[ESP+36]
    MOV     ECX,[ESP+40]
    MOV     EDX,[ESP+44]
    MOV     EBX,[ESP+48]
    MOV     EBP,[ESP+52]
    MOV     [EBP],ESP
    MOV     [EBP+4],SS
    MOV     ES,BX
    MOV     DS,BX
    MOV     FS,BX
    MOV     GS,BX

    OR      ECX,3
    OR      EBX,3
    PUSH    EBX
    PUSH    EDX
    PUSH    ECX
    PUSH    EAX
    RETF
