#include "platform.h"
#include "uart.h"
#include "printf.h"
#include "constants.h"

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
	// Return LED register
	return (uint32_t*)LEDS_ADDR;
}

uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	return LEDS_VAL;
}

void platform_trigger_swi()
{

}

void platform_clear_swi()
{

}
