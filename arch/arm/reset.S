.arm
.text

.globl reset_handler
reset_handler:
	bl setup_stacks
	bl load_vectors
	
	bl enable_caches
	
	bl arch_init
	bl platform_init
	
	bl harness_init
	bl harness_main
	
	bl platform_shutdown
1:
	b 1b
.size reset_handler, .-reset_handler

enable_caches:
	// Enable only I$ here. D$ can only be enabled when MMU is enabled
	// Need to set the Z, I (11, 12) of system control register
	
	mrc p15, 0, r0, cr1, cr0, 0
//	orr r0, #(1 << 2)  // Cache
//	orr r0, #(1 << 3)  // Write buffer
	orr r0, #(1 << 11) // Branch Prediction
	orr r0, #(1 << 12) // L1 instruction cache
//	orr r0, #(1 << 26) // L2 unified cache
	mcr p15, 0, r0, cr1, cr0, 0
	bx lr
.ltorg

setup_stacks:
	//Back up old cpsr
	mrs r1, cpsr
	
	//Set up stack for USR mode (can't do this in hardware)
	ldr r0, =stack_top
	str r0, [r0]
	ldm r0, {sp}^
	mov r2, lr
	
	//Set up stack for IRQ mode
	mov r0, #0x12
	msr cpsr_c, r0
	ldr sp, =irq_stack_top
	
	//Set up stack for PABT mode
	mov r0, #0x17
	msr cpsr_c, r0
	ldr sp, =pabt_stack_top
	
	//Set up stack for SWI mode
	msr cpsr_c, r1
	ldr sp, =swi_stack_top
	
	//Switch in to SYS mode
	orr r1, #0xf
	msr cpsr_c, r1
	
	bx r2
.size setup_stacks,.-setup_stacks


// Load vectors to location given by linker script (_VECTORS_RELOCATE)
.extern _VECTORS_RELOCATE
.extern vectors_start
.extern vectors_end
load_vectors:
	mov r5, lr
	ldr r0, =_VECTORS_RELOCATE
	
	ldr r1, =vectors_start
	ldr r2, =vectors_end
	sub r2, r1
	
	bl memcpy
	
// Also switch on relocated vectors
	mrc p15, 0, r0, cr1, cr0, 0
	orr r0, #(1 << 13)
	mcr p15, 0, r0, cr1, cr0, 0
	
	bx r5


.data
.align 8
stack_bottom:
.space (1024*1024) - 4
stack_top:
.word 0

.align 8
irq_stack_bottom:
.space 2044
irq_stack_top:
.word 0

.align 8
pabt_stack_bottom:
.space 2044
pabt_stack_top:
.word 0

.align 8
swi_stack_bottom:
.space 2044
swi_stack_top:
.word 0	
