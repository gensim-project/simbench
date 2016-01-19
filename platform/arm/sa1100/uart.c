#include "uart.h"
#include "constants.h"

void uart_init()
{
	
}

void uart_putc(char c)
{
	volatile uint32_t* data_ptr = (uint32_t*)((UART3_BASE) + 0x14);
	*data_ptr = c;
}
