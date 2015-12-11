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
