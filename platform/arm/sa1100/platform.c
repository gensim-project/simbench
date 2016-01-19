#include "platform.h"
#include "printf.h"
#include "uart.h"
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
	// Return real time clock count register (RCNR)
	return (uint32_t*)0x90010004;
}

uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	return 0x0;
}

#define PL190_BASE 0x10140000

// The SA1100 interrupt controller does not support a software 
// interrupt. For this reason, we set up a 1-tick counter instead.
// This makes the result of this test somewhat incomparable for 
// Simit-ARM

void platform_trigger_swi()
{
	// Enable OS timer clock interrupt on interrupt controller
	*(volatile uint32_t *)(IC_START + 0x4) = (1 << 26);
	
	// Enable OS timer clock interrupt on OS timer
	*(volatile uint32_t *)(OSTIMER_START + 0x1C) = 0x1;
	
	// Set up OS timer to trigger interrupt on next tick
	*(volatile uint32_t *)(OSTIMER_START + 0x0) = 0x1;
	
	// Set OS timer value
	*(volatile uint32_t *)(OSTIMER_START + 0x10) = 0x0;
}

void platform_clear_swi()
{
	
	// Clear interrupt on OS timer
	*(volatile uint32_t *)(OSTIMER_START + 0x14) = 0x1;
	
	// Disable interrupt
	*(volatile uint32_t *)(OSTIMER_START + 0x1C) = 0x0;
}
