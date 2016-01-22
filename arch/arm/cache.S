/*****
*
*  cache.S
*  Cache maintenance operations for armv7
*  Based on code from the Linux kernel (arch/arm/mm/cache-v7.S)
*
******/
.text
.arm
.syntax unified

.globl mem_arm_flush_dcache
mem_arm_flush_dcache:
	push {r4-r7, r9-r11}
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	mov	r3, r0, lsr #23			@ move LoC into position
	ands	r3, r3, #7 << 1			@ extract LoC*2 from clidr
	beq	finished			@ if loc is 0, then no need to clean
start_flush_levels:
	mov	r10, #0				@ start clean at cache level 0
flush_levels:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	skip				@ skip if no cache, or just i-cache

	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr

	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	movw	r4, #0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	movw	r7, #0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
loop1:
	mov	r9, r7				@ create working copy of max index
loop2:
    orr	r11, r10, r4, lsl r5		@ factor way and cache number into r11
    orr	r11, r11, r9, lsl r2		@ factor index number into r11
 	mcr	p15, 0, r11, c7, c14, 2		@ clean & invalidate by set/way
	subs	r9, r9, #1			@ decrement the index
	bge	loop2
	subs	r4, r4, #1			@ decrement the way
	bge	loop1
skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	flush_levels
finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb	st
	isb
	pop {r4-r7, r9-r11}
	bx lr


.extern mem_cache_flush
.globl mem_arm_cache_mmu_off
mem_arm_cache_mmu_off:

	dmb
	dsb
	
	// Disable the MMU, D$, Write buffer
	mrc p15, 0, r0, c1, c0 
	bic r0, r0, #0xd
	mcr p15, 0, r0, c1, c0 
	mov r12, lr
	// Flush the caches
	bl mem_arm_flush_dcache
	
	mov r0, #0
	mcr p15, 0, r0, c8, c7, 0 // TLB Flush
	mcr p15, 0, r0, c7, c5, 6 // BTB flush
	mcr p15, 0, r0, c7, c10, 4 // DSB
	mcr p15, 0, r0, c7, c5, 4 // ISB
	mov pc, r12
