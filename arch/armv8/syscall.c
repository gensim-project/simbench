#include "syscall.h"

void armv8_syscall(int call_no) {
	asm volatile ("svc #1");
}
