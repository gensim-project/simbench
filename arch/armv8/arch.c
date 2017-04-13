#include "arch.h"
#include "debug.h"
#include "printf.h"

void arch_init()
{
	// Enable ASIMD unit
	// modify cpacr.fpen
	uint32_t fpen = (1 << 20) | (1 << 21);
	asm volatile ("msr CPACR_EL1, %0" :: "r"(fpen));
	
	// Install syscall handler
	armv8_install_syscall_handler();
	
	armv8_heap_init();
}

void arch_abort()
{
	fprintf(OUTPUT, "armv8: abort!\n");
	while(1) ;
}

void arch_code_flush(size_t address)
{
	arch_abort();
}

uint32_t arch_nonpriviliged_read(volatile uint32_t *ptr)
{
	arch_abort();
}

void arch_undefined_instruction()
{
	asm volatile (".inst 0x00000000");
}

void arch_syscall()
{
	arch_abort();
}

void arch_coprocessor_access()
{
	arch_abort();
}

