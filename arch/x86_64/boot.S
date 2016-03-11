.code32
#define DEBUG_BOOT
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003

#define INIT_PGT	0x08000

.section ".bootstrap", "ax"

.globl multiboot_start
multiboot_start:
	// Disable interrupts
	cli

	// Begin 32-bit execution
	jmp start32

// Multiboot header - should be as close to the top as practicable
.align 4
__multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

.extern _HEAP_START
.extern _HEAP_SIZE

.align 16
start32:
	lea _HEAP_START, %ebp
	lea _HEAP_SIZE(%ebp), %ebp
	movl %ebp, %esp

	// Reset EFLAGS
	pushl $0
	popf

	// Store EBX and EAX.  Need the zeroes, because when we pop, they'll be popped as 64-bit
	pushl $0
	pushl %ebx
	pushl $0
	pushl %eax

	cld
	call clear_screen

#ifdef DEBUG_BOOT
	lea msg_disable_irq, %esi
	call printstr32
#endif

	// Disable IRQs
	mov $0xff, %al
	out %al, $0xa1
	out %al, $0x21

	// Let the PIC settle down
	nop
	nop

/*	mov %cr0, %edx
	std

	mov $0xb800e, %edi
	call pr_hex

1:
	jmp 1b

pr_hex:
	mov $8, %esi
	mov $0xf00, %ax

1:
	test %esi, %esi
	jz 3f

	mov %dl, %al
	shr $4, %edx
	and $0xf, %al
	add $0x30, %al
	cmp $0x39, %al
	jbe 2f
	add $0x7, %al

2:
	stosw
	dec %esi
	jmp 1b

3:
	ret*/

#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea msg_prep_pgt, %esi
	call printstr32
#endif

	// Load a NULL IDT
	lidt idt_ptr

#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea msg_build_pgt, %esi
	call printstr32
#endif

	// Build the initial page tables
	mov $(INIT_PGT), %edi
	mov %edi, %cr3

	call build_pagetables

#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea msg_sysreg, %esi
	call printstr32
#endif

	// Initialise CR4 with: PSE, PAE, PGE, OSFXSR, OSXMMEXCPT
	mov $0x6b0, %eax
	mov %eax, %cr4

	// Read the EFER
	mov $0xc0000080, %ecx
	rdmsr

	// Initialise EFER with: LME
	or $0x901, %eax
	wrmsr

	// Initialise CR0 with: PE, MP, EM, WP, PG
	mov %cr0, %eax
	or $0x80010007, %eax
	mov %eax, %cr0

	// Load the 64-bit GDT
	lgdt %cs:gdt_ptr

#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea msg_jump64, %esi
	call printstr32
#endif

	// Chump to the 64-bit entry-point
	ljmp $0x08, $start64

build_pagetables:	// EDI = PML4
	mov %edi, %ebx
	cld

	// Clear out the PML4
	lea (%ebx), %edi
	xor %eax, %eax
	mov $0x400, %ecx
	rep stosl

	// PML4 - Single entry
	lea 0x1000(%ebx), %edx
	or $3, %edx
	mov %edx, (%ebx)

	// Clear out the PDPT
	lea 0x1000(%ebx), %edi
	xor %eax, %eax
	mov $0x400, %ecx
	rep stosl

	// Detect whether or not we can use 1Gb pages
	mov $0x80000001, %eax

	push %ebx		// JIC
	cpuid
	pop %ebx		// JIC

	andl $(1<<26), %edx

	jz pt2mb

#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea using_1gb, %esi
	call printstr32
#endif

	// Insert 1GiB page entry into PDP[0]
	movb $0x83, 0x1000(%ebx)

	ret

pt2mb:
#ifdef DEBUG_BOOT
	// Print out a debugging message
	lea using_2mb, %esi
	call printstr32
#endif

	// PD
	lea 0x2000(%ebx), %edx
	or $0x3, %edx
	mov %edx, 0x1000(%ebx)

	lea 0x2000(%ebx), %edi
	xor %eax, %eax
	mov $0x400, %ecx
	rep stosl

	// PD - Map a bunch of 2MiB pages
	mov $0x83, %eax
	xorl %ecx, %ecx

1:
	mov %eax, 0x2000(%ebx, %ecx, 8)
	add $0x200000, %eax

	incl %ecx
	cmp $0x200, %ecx
	jne 1b

	ret

clear_screen:
	mov $0xb8000, %edi
	mov $((25*80*2)/4), %ecx
	xor %eax, %eax

	rep stosl

	ret

#ifdef DEBUG_BOOT
printstr_x: .long 0
printstr_y: .long 0

// Clobbers EAX and EDI
printstr32:
	movl printstr_x, %eax
	movl printstr_y, %edi
	leal (%edi, %edi, 4), %edi
	shl $0x4, %edi
	add %eax, %edi
	lea 0xb8000(%edi, %edi, 1), %edi
	mov $0x0e00, %ax

1:
	lodsb
	test %al, %al
	jz 3f

	cmp $0x0a, %al
	jne 2f

	movl $0, printstr_x
	incl printstr_y
	jmp printstr32

2:
	stosw
	incl printstr_x
	jmp 1b

3:
	ret
#endif

.align 16

.code64
start64:
	// Load the segment registers with the data segment selector
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	//call clear_screen64

	// Zero out the BSS
	mov $_BSS_START, %rdi
	mov $_BSS_END, %rcx
	sub %rdi, %rcx
	shr $3, %ecx
	xor %eax, %eax
	rep stosq

	// Grab the saved multiboot arguments
	pop %rdi
	pop %rsi

	// Push a canary on to the stack
	mov $0xdeadbeefbabecafe, %rax
	push %rax

	// Begin!
	sti
	call arch_start

	// If we get here, loop forever.
1:
	hlt
	pause
	jmp 1b

clear_screen64:
	lea 0xb8000, %rdi
	mov $0x1f4, %ecx

	xor %eax, %eax

	cld
	rep stosq

	ret

.data

#ifdef DEBUG_BOOT
msg_disable_irq: .asciz "Disabling interrupts...\n"
msg_prep_pgt: .asciz "Preparing page-tables...\n"
msg_build_pgt: .asciz "Building page-tables...\n"
msg_sysreg: .asciz "Populating system registers...\n"
msg_jump64: .asciz "Jumping to 64-bit mode...\n"
msg3: .asciz "Hello from 64-bit mode!\n"
using_2mb: .asciz "Using 2mb pages\n"
using_1gb: .asciz "Using 1gb pages\n"
#endif

.align 16
gdt_ptr:
	.word __gdt_end - __gdt
	.long __gdt

idt_ptr:
	.word 0
	.long 0

.align 16
__gdt:
	// NULL
	.quad 0

	// Kernel CS
	.quad 0x00209A0000000000

	// Kernel DS
	.quad 0x0000920000000000
__gdt_end:

