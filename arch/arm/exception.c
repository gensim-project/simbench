#include "arch.h"

extern uint32_t vectors_start;
extern uint32_t _VECTORS_RELOCATE;
extern uint32_t pabt_handler;
extern uint32_t dabt_handler;
extern uint32_t swi_handler;
extern void pabt_return();
extern void dabt_skip();
extern void swi_skip();
extern void default_handler();

// The vectors have been copied into the low page so need to do some address calculation
static void install_pabt(uint32_t pabt_location)
{
	uint32_t vector_location = ((uint32_t)&pabt_handler - (uint32_t)&vectors_start) + (uint32_t)&_VECTORS_RELOCATE;
	*(uint32_t*)vector_location = pabt_location;
}
static void install_dabt(uint32_t dabt_location)
{
	uint32_t vector_location = ((uint32_t)&dabt_handler - (uint32_t)&vectors_start) + (uint32_t)&_VECTORS_RELOCATE;
	*(uint32_t*)vector_location = dabt_location;
}
static void install_swi(uint32_t swi_location)
{
	uint32_t vector_location = ((uint32_t)&swi_handler - (uint32_t)&vectors_start) + (uint32_t)&_VECTORS_RELOCATE;
	*(uint32_t*)vector_location = swi_location;
}

void arch_ifault_install_return()
{
	install_pabt((uint32_t)pabt_return);
}

void arch_ifault_install_break()
{
	install_pabt((uint32_t)default_handler);
}

void arch_dfault_install_skip()
{
	install_dabt((uint32_t)dabt_skip);
}

void arch_syscall_install_skip()
{
	install_swi((uint32_t)swi_skip);
}
