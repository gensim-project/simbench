#include "uart.h"

#include <stdint.h>

typedef void (*ctor_t)();
extern intptr_t _CTORS_START, _CTORS_END;

void harness_main()
{
	intptr_t *ctor_ptr;

	for(ctor_ptr=&_CTORS_START; ctor_ptr != &_CTORS_END; ctor_ptr++)
	{
		ctor_t ctor = (ctor_t)*ctor_ptr;
		ctor();
	}
	
	
}
