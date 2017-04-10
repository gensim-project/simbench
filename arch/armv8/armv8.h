#ifndef ARMV8_ARMV8_H
#define ARMV8_ARMV8_H

#include "syscall.h"

typedef int (*exception_handler_t)();

void armv8_install_sync_handler(int ec, exception_handler_t handler);
int armv8_get_current_el();
int armv8_in_el1();

void armv8_syscall(int call_no);

#endif
