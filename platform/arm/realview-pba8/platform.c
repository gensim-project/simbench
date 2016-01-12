#include "platform.h"
#include "uart.h"
#include "printf.h"

void platform_init()
{
	uart_init();
	printf_register_output(uart_putc);
	printf_register_debug(uart_putc);
}

void platform_shutdown()
{
	while(1);
}

uint32_t* platform_get_null_devptr()
{
	// Return SP810 LED register
	return (uint32_t*)0x10000008;
}

uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	return 0xff;
}

#define PL190_BASE 0x10140000
void platform_trigger_swi()
{
	//Enable interrupt channel 0
	*(volatile uint32_t*)(PL190_BASE + 0x10) = 0x1;
	
	//Trigger a SWI on the PL190 interrupt controller
	*(volatile uint32_t*)(PL190_BASE + 0x18) = 0x1;
}

void platform_clear_swi()
{
	*(volatile uint32_t*)(PL190_BASE + 0x1C) = 0x1;
}
