#ifndef DEBUG_H
#define DEBUG_H

#include "define.h"

#ifdef SIMBENCH_DEBUG

#include "printf.h"

void debug_spinner_start(uint64_t counter_rate);
void debug_spinner();

#define dprintf(a...) fprintf(DEBUG, ##a)

#else

#define debug_spinner_start(x) do {} while(0)
#define debug_spinner() do {} while(0)
#define dprintf(a...) do {} while(0)

#endif

#endif
