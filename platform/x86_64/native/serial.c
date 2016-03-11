#include "serial.h"

#define COM1	0x3f8

static inline void out8(uint16_t port, uint8_t data)
{
	asm volatile("outb %0, %1" :: "a"(data), "d"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile("inb %1, %0" : "=a"(data) : "d"(port));

	return data;
}

void serial_init()
{
	out8(COM1 + 1, 0x00);
	out8(COM1 + 3, 0x80);
	out8(COM1 + 0, 0x03);
	out8(COM1 + 1, 0x00);
	out8(COM1 + 3, 0x03);
	out8(COM1 + 2, 0xc7);
	//out8(COM1 + 4, 0x0b);
}

static inline int is_transmit_empty()
{
	return !!(inb(COM1 + 5) & 0x20);
}

void serial_putc(char ch)
{
	while (!is_transmit_empty()) asm volatile("pause\n");

	out8(COM1, ch);
}
