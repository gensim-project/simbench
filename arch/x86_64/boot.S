.code32

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003
#define TEMP_STACK_SIZE 0x1000

#define TEMP_PGT 0x10000

.section ".bootstrap", "ax"

.globl multiboot_start
multiboot_start:
	// Disable interrupts
	cli

	// Disable IRQs
	mov $0xff, %al
	out %al, $0xa1
	out %al, $0x21

	nop
	nop

	// Begin
	jmp start32

.align 16
start32:
	movl $(__temp_stack + TEMP_STACK_SIZE), %esp

	// Reset EFLAGS
	pushl $0
	popf

	// Store EBX and EAX.  Need the zeroes, because when we pop, they'll be popped as 64-bit
	pushl %ebx
	pushl $0
	pushl %eax
	pushl $0

	// Print out a debugging message
	lea msg1, %esi
	call printstr32

	// Load a NULL IDT
	lidt idt_ptr

	mov $0x6b0, %eax
	mov %eax, %cr4
 
	// Build the initial page tables
	call build_pagetables
	mov %edi, %cr3
 
	// Read the EFER
	mov $0xc0000080, %ecx 
	rdmsr
	
	// Write the EFER with the LME bit set
	or $0x901, %eax
	wrmsr
 
	// Enable paging and protection
	mov %cr0, %eax
	or $0x80010007, %eax
	mov %eax, %cr0                    

	// Load the 64-bit GDT
	lgdt %cs:gdt_ptr

	// Chump to the 64-bit entry-point
	jmp $0x08, $start64

build_pagetables:
	mov $0x10000, %edi

	// PML4
	lea 0x1000(%edi), %eax
	or $3, %eax
	mov %eax, (%edi)

	// PDPT
	lea 0x2000(%edi), %eax
	or $3, %eax
	mov %eax, 0x1000(%edi)

	// PDP
	xor %eax, %eax
	or $0x83, %eax
	mov %eax, 0x2000(%edi)

	or 0x200000, %eax
	mov %eax, 0x2008(%edi)

	ret

printstr32:
	movb (%esi), %al
	cmpb $0, %al
	jz 1f

	outb %al, $0xe9
	inc %esi
	jmp printstr32

1:
	ret

.align 16

.code64
start64:
	lea msg2, %rdi
	call printstr64

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	pop %rsi
	pop %rdi
	call arch_start

1:
	hlt
	jmp 1b

.globl printstr64
printstr64:
	movb (%rdi), %al
	cmpb $0, %al
	jz 1f

	outb %al, $0xe9
	inc %rdi
	jmp printstr64

1:
	ret

.align 4
__multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

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

.align 4096
.comm __temp_stack, TEMP_STACK_SIZE

