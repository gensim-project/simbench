.text

.globl reset_handler
reset_handler:
	bl setup_stacks
	bl load_vectors
	bl enable_caches
		
	bl arch_init
	bl platform_init
	
	ldr X0, =run_simbench
	msr ELR_EL1, X0 
	eret
	
.size reset_handler,.-reset_handler

setup_stacks:
	ldr X0, =el0_stack_top
	msr sp_el0, X0
	ldr X0, =el1_stack_top
	mov sp, X0
	ret

load_vectors:
	ret

enable_caches:
	ret

run_simbench:
	bl harness_init
	bl harness_main
	
	bl platform_shutdown

.data
el0_stack:
	.space 4096
el0_stack_top:

el1_stack:
	.space 4096
el1_stack_top:
