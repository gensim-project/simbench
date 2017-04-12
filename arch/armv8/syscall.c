#include "arch.h"
#include "define.h"
#include "syscall.h"

static int global_call_no;

void armv8_syscall(int call_no) {
	global_call_no = call_no;
	asm volatile ("svc #1");
}

static void syscall_enable_irq() {
	uint32_t daif;
	asm volatile ("mrs %0, DAIF" : "=r"(daif));
	daif &= ~(1 << 1);
	asm volatile("msr DAIF, %0" :: "r"(daif));
}

static void syscall_disable_irq() {
	uint32_t daif;
	asm volatile ("mrs %0, DAIF" : "=r"(daif));
	daif |= (1 << 1);
	asm volatile("msr DAIF, %0" :: "r"(daif));
}

int armv8_handle_simbench_syscall() {
	switch(global_call_no) {
		case SYSCALL_IRQ_ENABLE:
			syscall_enable_irq();
			break;
		case SYSCALL_IRQ_DISABLE:
			syscall_disable_irq();
			break;
		default:
			arch_abort();
			break;
	}
	
	return 0;
}
