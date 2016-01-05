#include "arch.h"

void arch_init()
{
	
}

void arch_code_flush(size_t address)
{
	// From ARM Architecture Ref Manual page 678
	
	// Clean data cache by MVA
	asm("mcr p15, 0, %0, c7, c6, 1" :: "r"(address));
	
	// DSB
	asm("mcr p15, 0, %0, c7, c10, 4" :: "r"(0));
	
	// Invalidate ICache by MVA
	asm("mcr p15, 0, %0, c7, c5, 1" :: "r"(address));
	
	// Invalidate BTB by MVA
	// DSB
	asm("mcr p15, 0, %0, c7, c10, 4" :: "r"(0));
	// PrefetchFlush
	asm("mcr p15, 0, %0, c7, c5, 4" :: "r"(0));
}

uint32_t arch_nonpriviliged_write(uint32_t *ptr)
{
	uint32_t data;
	asm("ldrt %0, [%1]" : "=&r"(data) : "r"(ptr));
	return data;
}

void arch_undefined_instruction()
{
	asm(".word 0xe7ffffff\n");
}
