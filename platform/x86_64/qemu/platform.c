#include "platform.h"
#include "uart.h"
#include "printf.h"

#define ROWS	25
#define COLUMNS	80

static void clear_screen()
{
	uint16_t *vga = (uint16_t *)0xb8000;
	
	for (int i = 0; i < ROWS * COLUMNS; i++) {
		vga[i] = 0;
	}
}

void platform_init()
{
	uart_init();
	printf_register_stdout(uart_putc);
	printf_register_uart(uart_putc);
	
	clear_screen();
}

void platform_shutdown()
{
	
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
