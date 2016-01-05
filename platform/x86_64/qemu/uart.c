#include "uart.h"
#include "arch.h"

void uart_init()
{
}

void uart_putc(char c)
{
	asm volatile("outb %0, $0xe9" :: "a"(c));
}

void uart_puts(const char *s)
{
	while(*s) {
		uart_putc(*s);
		s++;
	}
}

static char hexchars[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

void uart_puthex(uint32_t x)
{
	int32_t shift = 28;
	while(shift >= 0) {
		uint32_t val = x >> shift;
		val &= 0xf;
		
		uart_putc(hexchars[val]);
				
		shift -= 4;
	}
}
