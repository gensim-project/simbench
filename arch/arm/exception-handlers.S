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
	orr lr, #0x3
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

.ltorg

