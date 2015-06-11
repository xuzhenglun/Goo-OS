extern bootmain
 
[bits 16]
global start

start:
    ; 切换显示模式，320*200*8位彩色模式
    ; 来自于《30天自制操作系统》
    mov al, 0x13
    mov ah, 0x00
    int 0x10
 
    ; 下面代码来自于XV6
    cli
     
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
     
; 个人感觉这段开启A20的代码比网上的使用CALL的代码清晰、易懂
seta20.1:
    in al, 0x64
    test al, 0x2
    jnz seta20.1
     
    mov al, 0xd1
    out 0x64, al
     
seta20.2:
    in al, 0x64
    test al, 0x2
    jnz seta20.2
     
    mov al, 0xdf
    out 0x60, al
     
; 加载GDT
    lgdt [gdtdesc]
; 开启保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax
; 进入保护模式
    jmp dword 8:start32
     
[bits 32]
start32:
; 初始化非代码段寄存器
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0
    mov fs, ax
    mov gs, ax
     
; 设置栈顶
    mov esp, 0xc400
; 调用Ｃ语言中的函数
    call bootmain
     
spin:
    jmp spin
     
gdt:
    dw 0x0000, 0x0000, 0x0000, 0x0000
    dw 0xffff, 0x0000, 0x9a00, 0x00cf    ; 代码段 
    dw 0xffff, 0x0000, 0x9200, 0x00cf    ; 数据段
     
gdtdesc:
    dw (gdtdesc - gdt - 1)
    dd gdt
