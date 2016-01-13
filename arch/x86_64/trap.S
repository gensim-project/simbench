
.macro push_regs
	push %rsp
	pushf
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rdi
	push %rsi
	push %rbp
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15
.endm

.macro pop_regs
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rbp
	pop %rsi
	pop %rdi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax
	popf
	add $8, %rsp
.endm

.macro enter_intr,has_arg
.if ! \has_arg
	push $0
.endif
	push_regs

	mov %rsp, %rdi
.endm

.macro exit_intr
	pop_regs
	add $8, %rsp
	iretq
.endm

.macro trap_macro name,has_arg
.extern handle_trap_\name
.globl trap_\name
trap_\name:
	enter_intr \has_arg
	call handle_trap_\name
	exit_intr
.endm

trap_macro unknown,0
trap_macro unknown_arg,1

.extern syscall_handler_fn
.globl trap_syscall
trap_syscall:
	enter_intr 0

	lea syscall_handler_fn, %rcx
	call *(%rcx)

	exit_intr

.extern page_fault_handler_fn
.globl trap_page_fault
trap_page_fault:
	enter_intr 1

	mov %cr2, %rsi
	lea page_fault_handler_fn, %rcx
	call *(%rcx)

	exit_intr

.extern itw_handler_fn
.globl trap_irq
trap_irq:
	enter_intr 0

	lea irq_handler_fn, %rcx
	call *(%rcx)

	exit_intr
