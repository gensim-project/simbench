#include "platform.h"
#include "printf.h"
#include "console.h"
#include "arch.h"
#include "debug.h"
#include "serial.h"
#include "timer.h"

extern void lapic_issue_interrupt(int irq);
extern void lapic_acknowledge_irq(void);

extern int use_framebuffer;

void platform_init()
{
	console_init();
	serial_init();

	if (use_framebuffer)
		printf_register_output(console_putc);
	else
		printf_register_output(serial_putc);

	printf_register_debug(console_putc);
	printf_register_error(console_putc);

	timer_init();
}

void platform_shutdown()
{

}

uint32_t* platform_get_null_devptr()
{
	return (uint32_t*)0x0000fee00380ULL;
}

uint32_t platform_get_null_devval()
{
	return 0x1234;
}

void platform_trigger_swi()
{
	lapic_issue_interrupt(0x30);
}

void platform_clear_swi()
{
	lapic_acknowledge_irq();
}
