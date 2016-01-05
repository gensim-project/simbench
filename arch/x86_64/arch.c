#include "arch.h"
#include "platform.h"
#include "harness.h"
#include "printf.h"
#include "irq.h"

void arch_init()
{
	heap_init();
	irq_init();
}

void arch_abort()
{
	printf("x86-64: abort!\n");
	for (;;) asm volatile("hlt\n");
}

void arch_code_flush(size_t address)
{
	asm volatile("wbinvd\n");
}

uint32_t arch_nonpriviliged_write(uint32_t *ptr)
{
	arch_abort();
}

extern char _HEAP_START;

void arch_start(unsigned int magic, void *mb_info)
{
	arch_init();
	platform_init();

	printf("x86-64: arch_start(magic=%08x, mb_info=%p), heap-start=%p\n", magic, mb_info, &_HEAP_START);

	harness_init();
	harness_main();
	platform_shutdown();
}
