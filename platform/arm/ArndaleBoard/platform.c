#include "platform.h"
#include "uart.h"
#include "printf.h"
#include "constants.h"

void platform_init()
{
	uart_init();
	printf_register_output(uart_putc);
	printf_register_debug(uart_putc);
	printf_register_error(uart_putc);
	
	// Disable watchdog timer
	volatile uint32_t* WTCON = (volatile uint32_t*)0x101D000;
	*WTCON = 0;
	
	// Set up GPIO for LEDs
	volatile uint32_t *GPY6CON = (volatile uint32_t*)(GPY6CON_ADDR);
	*GPY6CON = 0x11111111;
	volatile uint32_t *GPY6PUD = (volatile uint32_t*)(GPY6PUD_ADDR);
	*GPY6PUD = 0xffff;		
}

void platform_shutdown()
{
	while(1);
}

uint32_t* platform_get_null_devptr()
{	
	// Return LED register
	return GPY6DAT_ADDR;
}

static char toggle = 0;
uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	toggle = !toggle;
	return toggle ? 0xff : 0x00;
}

#define GICD_BASE 0x10481000
#define GICD_ISENABLER0 *(volatile uint32_t*)(GICD_BASE + 0x100)
#define GICD_SGIR *(volatile uint32_t*)(GICD_BASE + 0x0f00)
#define GICD_CPENDSGIR0 *(volatile uint32_t*)(GICD_BASE + 0xf10)
void platform_trigger_swi()
{
	// enable SGI0 on the GICD_ISENABLER0 register
	GICD_ISENABLER0 = 0x1;
	
	// direct SGI0 to CPU0 GICD_ITARGETSR0 register
	// (these are read only)
	
	// generate SGI0
	uint32_t targetlistfilter = 0x2;
	uint32_t cputargetlist = 0;
	uint32_t nsatt = 0;
	uint32_t sgiintid = 0;
	
	GICD_SGIR = sgiintid | (nsatt << 15) | (cputargetlist << 16) | (targetlistfilter << 24);
}

void platform_clear_swi()
{
	GICD_CPENDSGIR0 = 0xff;
}
