#include "arch.h"
#include "debug.h"
#include "printf.h"
#include "syscall.h"

void arch_init()
{
	// Enable ASIMD unit
	// modify cpacr.fpen
	uint32_t fpen = (1 << 20) | (1 << 21);
	asm volatile ("msr CPACR_EL1, %0" :: "r"(fpen));
	
	armv8_install_sync_handler(ARMV8_EC_SVC_A64, armv8_handle_simbench_syscall);
	
	// Enable I$ operations in EL0
	uint32_t sctlr_el1;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr_el1));
	sctlr_el1 |= (1 << 26); // SCTLR_EL1.UCI
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr_el1));
}

void arch_abort()
{
	fprintf(OUTPUT, "armv8: abort!\n");
	while(1) ;
}

void arch_code_flush(size_t address)
{
	asm volatile ("ic IVAU, %0" : : "r"(address));
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
	asm volatile("svc #0");
}

void arch_coprocessor_access()
{
	arch_abort();
}

