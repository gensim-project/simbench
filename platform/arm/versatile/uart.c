#include "uart.h"
#include "constants.h"

struct uart_regs_t
{
	volatile uint32_t dr;
	volatile uint32_t rsr;
	uint32_t padding1[3];
	volatile uint32_t fr;
	uint32_t padding2[2];
	volatile uint32_t ilpr;
	volatile uint32_t ibrd;
	volatile uint32_t fbrd;
	volatile uint32_t lcr_h;
	volatile uint32_t cr;
	volatile uint32_t ifls;
	volatile uint32_t imsc;
	volatile uint32_t ris;
	volatile uint32_t mis;
	volatile uint32_t icr;
	volatile uint32_t dmacr;
};

void uart_init()
{
	
}

void uart_putc(char c)
{
	struct uart_regs_t *regs = (struct uart_regs_t*)(UART0_BASE);
	regs->dr = c;
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
