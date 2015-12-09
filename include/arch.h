#ifndef ARCH_H
#define ARCH_H

#include "define.h"

extern void arch_init();

extern void arch_code_flush(size_t address);
uint32_t arch_nonpriviliged_write(uint32_t *ptr);

#endif
