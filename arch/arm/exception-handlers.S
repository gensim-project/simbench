.arm
.text

// pabt_return
// load the user LR and return to it
// TODO: this exception handler changes mode several times which is expensive. If we operated in usr mode instead of 
// svc mode we could use an stm^ instead of switching modes
.globl pabt_return
pabt_return:
	push {r0}
	
	mrs r0, CPSR
	bic lr, r0, #0xf
	orr lr, #0xf
	msr CPSR, lr
	mov r1, lr
	msr CPSR, r0
	mov lr, r1

	pop {r0}
	movs pc, lr
.ltorg

// dabt_skip
// skip the instruction accessing memory

.globl dabt_skip
dabt_skip:
	subs pc, lr, #4

.globl swi_skip
swi_skip:
	movs pc, lr

.globl undef_skip
undef_skip:
	movs pc, lr

.globl irq_handler_tramp
irq_handler_tramp:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	
	ldr r0, arch_irq_handler
	blx r0
	
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	movs pc, lr

.globl arch_irq_handler
arch_irq_handler:
.word 0x0

.ltorg

