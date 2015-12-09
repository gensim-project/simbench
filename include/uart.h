#ifndef UART_H
#define UART_H

#include "define.h"

extern void uart_init();
extern void uart_putc(char c);
extern void uart_puts(const char *c);

extern void uart_puthex(uint32_t hex);

#endif
