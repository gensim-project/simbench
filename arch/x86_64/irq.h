#ifndef IRQ_H
#define IRQ_H

#include "define.h"

struct mcontext;

typedef void (*syscall_handler_fn_t)(struct mcontext *);
typedef void (*page_fault_handler_fn_t)(struct mcontext *, uint64_t);

extern void irq_init();
extern void irq_install_syscall_handler(syscall_handler_fn_t handler_fn);
extern void irq_install_page_fault_handler(page_fault_handler_fn_t handler_fn);
extern void irq_reset_syscall_handler();
extern void irq_reset_page_fault_handler();

#endif /* IRQ_H */
