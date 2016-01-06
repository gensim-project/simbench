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
		printf("x86: unable to decode instruction @ %p\n", mcontext->rip);
		arch_abort();
	}
	
	mcontext->rip += inst.length;
}

void arch_dfault_install_skip()
{
	mem_install_page_fault_handler(skip_handler);
}
