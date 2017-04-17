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
	
	uint32_t sctlr;
	asm volatile ("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	// disable SP alignment checks
	sctlr &= ~((1 << 4) | (1 << 3));
	// enable IVAU at EL0
	sctlr |= (1 << 26);
	
	// enable caches
	sctlr |= (1 << 12); // I
	sctlr |= (1 << 2);  // C
	
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	// Install syscall handler
	armv8_install_syscall_handler();
	
	// enable CNTPCT for coprocessor
	uint32_t cntkctl;
	asm volatile ("mrs %0, CNTKCTL_EL1" : "=r"(cntkctl));
	cntkctl |= (1 << 0);
	asm volatile ("msr CNTKCTL_EL1, %0" :: "r"(cntkctl));
	
	armv8_heap_init();
}

void arch_abort()
{
	fprintf(OUTPUT, "armv8: abort!\n");
	while(1) ;
}

void arch_code_flush(size_t address)
{
	asm volatile ("ic IVAU, %0" :: "r"(address));
}

uint32_t arch_nonpriviliged_read(volatile uint32_t *ptr)
{
	uint32_t data;
	asm volatile ("ldtr %0, [%1]" : "=r"(data) : "r"(ptr));
	return data;
}

void arch_undefined_instruction()
{
	asm volatile (".inst 0x00000000");
}

void arch_syscall()
{
	asm volatile ("svc #0");
}

void arch_coprocessor_access()
{
	uint32_t CNTPCT;
	asm volatile("mrs %0, CNTPCT_EL0" : "=r"(CNTPCT));
}

void arch_priv_enter() {
	armv8_syscall(SYSCALL_PRIV_ENTER);
}

void arch_priv_leave() {
	armv8_syscall(SYSCALL_PRIV_LEAVE);
}
