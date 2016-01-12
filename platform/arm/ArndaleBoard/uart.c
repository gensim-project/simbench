#include "uart.h"
#include "constants.h"

#define TX_ERROR_MASK           0x8
#define TX_FIFO_FULL_MASK	(1 << 24)

struct uart_regs_t
{
	volatile uint32_t ulcon;
	volatile uint32_t ucon;
	volatile uint32_t ufcon;
	volatile uint32_t umcon;
	volatile uint32_t utrstat;
	volatile uint32_t uerstat;
	volatile uint32_t ufstat;
	volatile uint32_t umstat;
	volatile uint32_t utxh;
	volatile uint32_t urxh;
	volatile uint32_t ubrdiv;
	volatile uint32_t ufracval;
        volatile uint32_t uintp;
	volatile uint32_t uints;
	volatile uint32_t uintm;
};

void uart_init()
{
  // U-Boot init the UART for us.
}

void uart_putc(char c)
{

	struct uart_regs_t *regs = (struct uart_regs_t*)(TTY_UART_BASE);

	/* wait for room in the tx FIFO */
	while (((regs->ufstat) & TX_FIFO_FULL_MASK)) {
		if ((regs->uerstat) & TX_ERROR_MASK)
			return;
	}

	regs->utxh = c;
}
