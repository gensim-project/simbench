#include "define.h"
#include "syscall.h"

#define ARMV8_SCTLR_I_FLAG (1 << 12)

static int global_syscall_no;

// Trigger a SimBench syscall
void armv8_syscall(int call_no) {
	global_syscall_no = call_no;
	asm volatile ("svc #1");
}

static int syscall_irq_disable() {
	uint32_t sctlr;
	asm volatile ("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr |= ARMV8_SCTLR_I_FLAG;
	asm volatile ("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	return 0;
}

static int syscall_irq_enable() {
	uint32_t sctlr;
	asm volatile ("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr &= ~ARMV8_SCTLR_I_FLAG;
	asm volatile ("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	return 0;
}

int syscall_mem_init();
int syscall_mem_mmu_enable();
int syscall_mem_mmu_disable();

// Handle a SimBench syscall
int armv8_handle_syscall() {
	switch(global_syscall_no) {
		case SYSCALL_IRQ_DISABLE: 
			return syscall_irq_disable();
		case SYSCALL_IRQ_ENABLE: 
			return syscall_irq_enable();
		case SYSCALL_MEM_INIT:
			return syscall_mem_init();
		case SYSCALL_MMU_ENABLE:
			return syscall_mem_mmu_enable();
		case SYSCALL_MMU_DISABLE:
			return syscall_mem_mmu_enable();
		default:
			return 1;
	}
}
