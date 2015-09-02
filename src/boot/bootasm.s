extern bootmain
 
[bits 16]
global start

;BOOT INFO
CYLS     EQU     0x0ff0
LEDS     EQU     0x0ff1
VMODE    EQU     0x0ff2
SCRNX    EQU     0x0ff4
SCRNY    EQU     0x0ff6
VRAM     EQU     0x0ff8

VBEMODE	EQU		0x105			; 1024 x  768 x 8bitカラー

start:

; VBE存在確認

		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; VBEのバージョンチェック

		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320

; 画面モード情報を得る

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 画面モード情報の確認

		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; モード属性のbit7が0だったのであきらめる

; 画面モードの切り替え

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGAグラフィックス、320x200x8bitカラー
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; キーボードのLED状態をBIOSに教えてもらう

keystatus:

    MOV AH,0x02
    INT 0x16
    MOV [LEDS],AX

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
    jmp dword 16:start32
     
[bits 32]
start32:
; 初始化非代码段寄存器
    mov ax, 0x08
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0
    mov fs, ax
    mov gs, ax
     
; 设置栈顶
    mov esp, start
; 调用Ｃ语言中的函数
    call bootmain
     
spin:
    jmp spin
     
gdt:
    dw 0x0000, 0x0000, 0x0000, 0x0000
    dw 0xffff, 0x0000, 0x9200, 0x00cf    ; 数据段
	dw 0xffff, 0x0000, 0x9a00, 0x00cf    ; 代码段 
     
gdtdesc:
    dw (gdtdesc - gdt - 1)
    dd gdt
