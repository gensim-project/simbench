#include "arch.h"
#include "x86.h"
#include "decode.h"
#include "printf.h"

static void return_handler(struct mcontext *mcontext, uint64_t va)
{
	mcontext->rip = *(uint64_t *)(mcontext->tempSP);
	mcontext->tempSP += 8;
}

void arch_ifault_install_return()
{
	mem_install_page_fault_handler(return_handler);
}

void arch_ifault_install_break()
{
	arch_abort();
}

static void skip_handler(struct mcontext *mcontext, uint64_t va)
{
	struct instruction inst;

	if (!decode_instruction((const uint8_t *)mcontext->rip, &inst)) {
		int i;

		printf("x86: unable to decode instruction @ %p\n", mcontext->rip);
		printf("code: ");

		for (i = 0; i < 32; i++) {
			printf("%02x ", ((uint8_t *)mcontext->rip)[i]);
		}

		printf("\n");

		arch_abort();
	}

	mcontext->rip += inst.length;
}

void arch_dfault_install_skip()
{
	mem_install_page_fault_handler(skip_handler);
}

static void syscall_skip_handler(struct mcontext *mcontext)
{
	// NOP
}

void arch_syscall_install_skip()
{
	irq_install_syscall_handler(syscall_skip_handler);
}

void arch_syscall()
{
	asm volatile("int $0x80\n");
}
