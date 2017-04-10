.text

/* First, some helpers to help save/restore registers */
save_state:
	stp X0, X1, [SP, #-16]!
	stp X2, X3, [SP, #-16]!
	stp X4, X5, [SP, #-16]!
	stp X6, X7, [SP, #-16]!
	stp X8, X9, [SP, #-16]!
	stp X10, X11, [SP, #-16]!
	stp X12, X13, [SP, #-16]!
	stp X14, X15, [SP, #-16]!
	stp X16, X17, [SP, #-16]!
	stp X18, X19, [SP, #-16]!
	stp X20, X21, [SP, #-16]!
	stp X22, X23, [SP, #-16]!
	stp X24, X25, [SP, #-16]!
	stp X26, X27, [SP, #-16]!
	stp X28, X29, [SP, #-16]!
	ret

restore_state:
	ldp X28, X29, [SP], #16
	ldp X26, X27, [SP], #16
	ldp X24, X25, [SP], #16
	ldp X22, X23, [SP], #16
	ldp X20, X21, [SP], #16
	ldp X18, X19, [SP], #16
	ldp X16, X17, [SP], #16
	ldp X14, X15, [SP], #16
	ldp X12, X13, [SP], #16
	ldp X10, X11, [SP], #16
	ldp X8, X9, [SP], #16
	ldp X6, X7, [SP], #16
	ldp X4, X5, [SP], #16
	ldp X2, X3, [SP], #16
	ldp X0, X1, [SP], #16
	ret

.section ".vectors"

.extern reset_handler

/* Each vector is 16 instructions. */

el1_current_sp0_synchronous_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_sp0_irq_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_sp0_fiq_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_sp0_serror_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
/* Now for SPX */

el1_current_spx_synchronous_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_spx_irq_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_spx_fiq_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_current_spx_serror_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Now for exceptions taken from a lower EL (EL0) with AARCH64 */
el1_lower_synchronous_vector:
	stp X0, X30, [SP, #-16]!
	bl save_state
	bl el1_lower_sync_handler
	cmp x0, #0
	bne 1f
	bl restore_state
	ldp X0, X30, [SP], #16
	eret
1:	bl arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_lower_irq_vector:
	stp X0, X30, [SP, #-16]!
	bl save_state
	bl el1_lower_irq_handler
	cmp x0, #0
	bne 1f
	bl restore_state
	ldp X0, X30, [SP], #16
	eret
1:	bl arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_lower_fiq_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
el1_lower_serror_vector:
	b arch_abort
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
