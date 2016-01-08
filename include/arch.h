#ifndef ARCH_H
#define ARCH_H

#include "define.h"

extern void arch_init();
extern void arch_abort() __attribute__((noreturn));

extern void arch_code_flush(size_t address);
extern uint32_t arch_nonpriviliged_write(uint32_t *ptr);

extern void arch_ifault_install_return();
extern void arch_ifault_install_break();

extern void arch_dfault_install_skip();

extern void arch_syscall_install_skip();
extern void arch_syscall();


#endif
