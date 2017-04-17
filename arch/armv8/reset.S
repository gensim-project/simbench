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

.extern vector_table_top
load_vectors:
	ldr x0, =vector_table_top
	msr VBAR_EL1, x0
	ret

enable_caches:
	ret

run_simbench:
	bl harness_init
	bl harness_main
	
	bl platform_shutdown


.extern armv8_backup_lr
.globl armv8_pabt_return
armv8_pabt_return:
	ldr X0, =armv8_backup_lr
	ldr X0, [X0]
	msr ELR_EL1, X0
	mov X0, #0
	ret
.size armv8_pabt_return,.-armv8_pabt_return
