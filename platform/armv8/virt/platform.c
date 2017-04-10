#include "platform.h"
#include "printf.h"
#include "uart.h"

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
	return (uint32_t*)0;
}

uint32_t platform_get_null_devval()
{
	return 0;
}

void platform_trigger_swi()
{
	while(1) ;
}

void platform_clear_swi()
{
	while(1) ;
}
