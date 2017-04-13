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
	arch_abort();
}

uint32_t platform_get_null_devval()
{
	arch_abort();
}

void platform_trigger_swi()
{
	arch_abort();
}

void platform_clear_swi()
{
	arch_abort();
}
