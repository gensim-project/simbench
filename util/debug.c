#include "debug.h"
#include "uart.h"

#ifdef SIMBENCH_DEBUG

#define DELETE 0x08

static char spinner_chars[4] = { '|','/','-','\\' };
static uint32_t counter_rollover = 0;
static uint32_t counter = 0;
static uint32_t counter_index = 0;

void debug_spinner_start(uint64_t rollover)
{
	uart_putc(' ');
	counter = 0;
	counter_rollover = rollover;
	counter_index = 0;
}

void debug_spinner()
{
	counter++;
	if(counter >= counter_rollover) {
		counter = 0;
		counter_index++;

		uart_putc(DELETE);
		uart_putc(spinner_chars[counter_index & 0x3]);
	}
}

#endif
