#ifndef ARMV8_SYSCALL_H
#define ARMV8_SYSCALL_H

#define ARMV8_EC_SVC_A64 0x15

#define SYSCALL_IRQ_ENABLE 1
#define SYSCALL_IRQ_DISABLE 2

int armv8_handle_simbench_syscall();

#endif
