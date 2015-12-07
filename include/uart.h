#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init();
void uart_putc(char c);
void uart_puts(const char *c);

void uart_puthex(uint32_t hex);

#endif
