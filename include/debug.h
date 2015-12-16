#ifndef DEBUG_H
#define DEBUG_H

#include "define.h"

#ifdef SIMBENCH_DEBUG

void debug_spinner_start(uint64_t counter_rate);
void debug_spinner();

#else

#define debug_spinner_start(x) do {} while(0)
#define debug_spinner() do {} while(0)

#endif

#endif
