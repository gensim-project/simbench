#include "arch.h"

void arch_init()
{
	
}

void arch_abort()
{
	for (;;) asm volatile("hlt\n");
}

void arch_code_flush(size_t address)
{
	arch_abort();
}

uint32_t arch_nonpriviliged_write(uint32_t *ptr)
{
	arch_abort();
}

extern void printstr64(const char *msg);

void arch_start(unsigned long a, unsigned long b)
{
	printstr64("Hello from C!\n");
	arch_init();
}
