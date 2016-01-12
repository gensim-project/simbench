.section ".vectors"
.arm
.syntax unified

.global vectors_start
vectors_start:

reset_vector:
b reset_trampoline

undef_vector:
b undef_trampoline

swi_vector:
b swi_trampoline

pabt_vector:
b pabt_trampoline

dabt_vector:
b dabt_trampoline

reserved:
.word 0

irq_vector:
b irq_trampoline

fiq_vector:
b fiq_trampoline

.space 2048

.extern reset_handler

reset_trampoline:
	ldr r0, =reset_handler
	bx r0

undef_trampoline:
	ldr pc, undef_handler

swi_trampoline:
	ldr pc, swi_handler

pabt_trampoline:
	ldr pc, pabt_handler

dabt_trampoline:
	ldr pc, dabt_handler

irq_trampoline:
	ldr pc, irq_handler

fiq_trampoline:
	ldr pc, fiq_handler

.org 4096

.globl undef_handler
undef_handler:
.word default_undef_handler

.globl swi_handler
swi_handler:
.word default_handler

.globl pabt_handler
pabt_handler:
.word default_handler

.globl dabt_handler
dabt_handler:
.word default_handler

.globl irq_handler
irq_handler:
.word default_handler

.globl fiq_handler
fiq_handler:
.word default_handler

.globl default_handler
default_handler:
	mov r0, #5
	swi 0x123456
	b default_handler

.globl default_undef_handler
.extern fprintf
.extern ERROR
default_undef_handler:
	mov r5, lr
	ldr r0, =ERROR
	ldr r1, =2f
	sub r2, r5, #4
	bl fprintf
1:
	b 1b
2:
.asciz "Undefined instruction at address %p\r\n"

.global vectors_end
vectors_end:
	.word 0
