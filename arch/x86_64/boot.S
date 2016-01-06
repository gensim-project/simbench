.code32

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003

#define INIT_PGT	0x0c000

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

	// Disable IRQs
	mov $0xff, %al
	out %al, $0xa1
	out %al, $0x21

	// Let the PIC settle down
	nop
	nop

	// Print out a debugging message
	lea msg1, %edi
	call printstr32

	// Load a NULL IDT
	lidt idt_ptr

	// Build the initial page tables
	mov $(INIT_PGT), %edi
	mov %edi, %cr3

	call build_pagetables

	// Initialise CR4 with: PSE, PAE, PGE, OSFXSR, OSXMMEXCPT
	mov $0x6b0, %eax
	mov %eax, %cr4 

	// Read the EFER
	mov $0xc0000080, %ecx 
	rdmsr
	
	// Initialise EFER with: SCE, LME, NXE
	or $0x901, %eax
	wrmsr
 
	// Initialise CR0 with: PE, MP, EM, WP, PG
	mov %cr0, %eax
	or $0x80010007, %eax
	mov %eax, %cr0                    

	// Load the 64-bit GDT
	lgdt %cs:gdt_ptr

	// Chump to the 64-bit entry-point
	jmp $0x08, $start64

build_pagetables:
	// PML4 - Single entry
	lea 0x1000(%edi), %eax
	or $3, %eax
	mov %eax, (%edi)

	// PDPT - Map a 1GiB page
	mov $0x83, %eax
	mov %eax, 0x1000(%edi)

	ret

printstr32:
	movb (%edi), %al
	cmpb $0, %al
	jz 1f

	outb %al, $0xe9
	inc %edi
	jmp printstr32

1:
	ret

.align 16

.code64
start64:
	// Print out a friendly message
	lea msg2, %rdi
	call printstr64

	// Load the segment registers with the data segment selector
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	// Grab the saved multiboot arguments
	pop %rdi
	pop %rsi

	// Push a canary on to the stack
	mov $0xdeadbeefbabecafe, %rax
	push %rax

	// Begin!
	call arch_start

	// If we get here, loop forever.
1:
	hlt
	pause
	jmp 1b

printstr64:
	movb (%rdi), %al
	cmpb $0, %al
	jz 1f

	outb %al, $0xe9
	inc %rdi
	jmp printstr64

1:
	ret

.data

msg1: .asciz "About to enter long mode...\n"
msg2: .asciz "Hello from long mode!  Reloading segment registers...\n"

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

