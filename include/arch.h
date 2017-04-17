#ifndef ARCH_H
#define ARCH_H

#include "define.h"

extern void arch_init();
extern void arch_abort() __attribute__((noreturn));

extern void arch_code_flush(size_t address);
extern uint32_t arch_nonpriviliged_read(volatile uint32_t *ptr);

extern void arch_ifault_install_return();
extern void arch_ifault_install_break();

extern void arch_dfault_install_skip();

extern void arch_undef_install_skip();

extern void arch_syscall_install_skip();
extern void arch_syscall();

extern void arch_coprocessor_access();
extern void arch_undefined_instruction();

extern void arch_irq_enable();
extern void arch_irq_disable();

typedef void (*irq_handler_t)();
extern void arch_irq_install_handler(irq_handler_t);

extern void arch_priv_enter();
extern void arch_priv_leave();

#endif
