#include "platform.h"
#include "printf.h"
#include "console.h"
#include "arch.h"
#include "debug.h"
#include "serial.h"

extern void lapic_issue_interrupt(int irq);
extern void lapic_acknowledge_irq(void);

void platform_init()
{
	console_init();
	serial_init();

	printf_register_output(serial_putc);
	printf_register_debug(console_putc);
	printf_register_error(console_putc);
}

void platform_shutdown()
{

}

uint32_t* platform_get_null_devptr()
{
	return (uint32_t*)0x10000008;
}

uint32_t platform_get_null_devval()
{
	return 0xff;
}

void platform_trigger_swi()
{
	lapic_issue_interrupt(0x30);
}

void platform_clear_swi()
{
	lapic_acknowledge_irq();
}
