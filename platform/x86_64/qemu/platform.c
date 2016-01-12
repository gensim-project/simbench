#include "platform.h"
#include "printf.h"
#include "console.h"

static void qemu_output_putc(char ch)
{
	asm volatile("outb %0, $0xe9" :: "a"(ch));
}

void platform_init()
{
	printf_register_output(qemu_output_putc);

	console_init();
	
#ifdef SIMBENCH_DEBUG
	printf_register_debug(qemu_output_putc);
	printf_register_error(qemu_output_putc);
#else
	printf_register_debug(console_putc);
	printf_register_error(console_putc);
#endif
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
