#include "arch.h"
#include "debug.h"

void arch_init()
{
	
}

void arch_abort()
{
	dprintf("arm: abort!\n");
	while(1) ;
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

uint32_t arch_nonpriviliged_read(volatile uint32_t *ptr)
{
	uint32_t data;
	asm("ldrt %0, [%1]" : "=&r"(data) : "r"(ptr));
	return data;
}

void arch_undefined_instruction()
{
	asm(".word 0xe7f000f0\n");
}

void arch_syscall()
{
	asm("swi #0\n" :::"lr");
}

void arch_coprocessor_access()
{
	// read the DACR
	uint32_t data;
	asm volatile("mrc p15, 0, %0, cr3, cr0, 0" : "=r"(data) :);
}

// Don't need to do anything here since we run in a privileged mode
// by default.
void arch_priv_enter()
{
	
}

void arch_priv_leave() 
{
	
}
