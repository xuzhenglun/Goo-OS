;Tab = 4
GLOBAL io_out8
GLOBAL io_store_eflags
GLOBAL hlt
GLOBAL cli
GLOBAL io_load_eflags
[section .text]
io_out8:
    MOV  EDX,[ESP+4]
    MOV  AL,[ESP+8]
    OUT  DX,AL
    RET

hlt:
    HLT
    RET

cli:
    CLI
    RET

io_load_eflags:
    PUSHFD
    POP EAX
    RET

io_store_eflags:
    MOV EAX,[ESP+4]
    PUSH EAX
    POPFD
    RET
