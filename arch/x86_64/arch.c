#include "arch.h"
#include "printf.h"

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

static void qemu_debug_putch(int ch)
{
	asm volatile("out %0, $0xe9" :: "a"(ch));
}

void arch_start(unsigned long a, unsigned long b)
{
	printf_register_putch(qemu_debug_putch);
	printf("Hello, from C! a=%lx, b=%lx\n", a, b);
	arch_init();
}
