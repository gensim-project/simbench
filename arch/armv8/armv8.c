#include "armv8.h"
#include "define.h"

int armv8_get_current_el() {
	uint32_t el;
	asm volatile("mrs %0, CurrentEL" : "=r"(el));
	return el;
}

int armv8_in_el1() {
	return armv8_get_current_el() == 1;
}
