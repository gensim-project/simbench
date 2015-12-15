#include "platform.h"
#include "uart.h"

void platform_init()
{
	uart_init();
}

void platform_shutdown()
{
	while(1);
}

uint32_t* platform_get_null_devptr()
{
	// Return SP810 LED register
	return (uint32_t*)0x00000000;
}

uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	return 0xff;
}
