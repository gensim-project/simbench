#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "define.h"

extern void platform_init();
extern void platform_shutdown();

// Return a pointer to an unused device which is safe to write to. This will be used for evaluating
// device access performance
extern uint32_t* platform_get_null_devptr();
// Return a value which is safe to write to the pointer returned by platform_get_null_devptr().
extern uint32_t platform_get_null_devval();

#endif
