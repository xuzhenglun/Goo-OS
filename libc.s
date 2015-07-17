	.file	"libc.c"
	.intel_syntax noprefix
	.text
	.globl	hlt
	.type	hlt, @function
hlt:
.LFB2:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
#APP
# 5 "./src/boot/libc.c" 1
	.intel_syntax noprefix
# 0 "" 2
# 6 "./src/boot/libc.c" 1
	hlt
# 0 "" 2
#NO_APP
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	hlt, .-hlt
	.globl	cli
	.type	cli, @function
cli:
.LFB3:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
#APP
# 10 "./src/boot/libc.c" 1
	.intel_syntax noprefix
# 0 "" 2
# 11 "./src/boot/libc.c" 1
	cli
# 0 "" 2
#NO_APP
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	cli, .-cli
	.globl	io_out8
	.type	io_out8, @function
io_out8:
.LFB4:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	edx, edi
	mov	eax, esi
	mov	WORD PTR [rbp-4], dx
	mov	BYTE PTR [rbp-8], al
#APP
# 15 "./src/boot/libc.c" 1
	.intel_syntax noprefix
# 0 "" 2
#NO_APP
	movzx	edx, WORD PTR [rbp-4]
	movzx	eax, BYTE PTR [rbp-8]
#APP
# 16 "./src/boot/libc.c" 1
	out dx,al
# 0 "" 2
#NO_APP
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	io_out8, .-io_out8
	.globl	io_load_eflags
	.type	io_load_eflags, @function
io_load_eflags:
.LFB5:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
#APP
# 23 "./src/boot/libc.c" 1
	.intel_syntax noprefix
# 0 "" 2
#NO_APP
	mov	DWORD PTR [rbp-4], 0
#APP
# 25 "./src/boot/libc.c" 1
	pushfd
pop eax
# 0 "" 2
#NO_APP
	mov	DWORD PTR [rbp-4], eax
	mov	eax, DWORD PTR [rbp-4]
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	io_load_eflags, .-io_load_eflags
	.globl	io_store_eflags
	.type	io_store_eflags, @function
io_store_eflags:
.LFB6:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	DWORD PTR [rbp-4], edi
#APP
# 33 "./src/boot/libc.c" 1
	.intel_syntax noprefix
# 0 "" 2
#NO_APP
	mov	eax, DWORD PTR [rbp-4]
#APP
# 34 "./src/boot/libc.c" 1
	PUSH eax
POPFD
# 0 "" 2
#NO_APP
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	io_store_eflags, .-io_store_eflags
	.ident	"GCC: (Debian 4.9.2-10) 4.9.2"
	.section	.note.GNU-stack,"",@progbits
