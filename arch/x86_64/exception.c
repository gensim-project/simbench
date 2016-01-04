#include "arch.h"

void arch_ifault_install_return()
{
	arch_abort();
}

void arch_ifault_install_break()
{
	arch_abort();
}

void arch_dfault_install_skip()
{
	arch_abort();
}
