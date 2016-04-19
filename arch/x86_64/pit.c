#include "define.h"
#include "printf.h"

#define PIT_PORT	0x61

void pit_init_oneshot(uint16_t x)
{
	uint8_t data;
	asm volatile("inb $0x61, %0" : "=a"(data));
	data &= 0xfd;
	data |= 1;
	asm volatile("outb %0, $0x61" :: "a"(data));

	asm volatile("outb %0, $0x43" :: "a"((uint8_t)0xb2));

	asm volatile("outb %0, $0x42" :: "a"((uint8_t)x));
	asm volatile("outb %0, $0x42" :: "a"((uint8_t)((x >> 8))));
}

void pit_start()
{
	uint8_t data;
	asm volatile("inb $0x61, %0" : "=a"(data));
	data &= 0xfe;
	asm volatile("outb %0, $0x61" :: "a"(data));
	data |= 1;
	asm volatile("outb %0, $0x61" :: "a"(data));
}

void pit_stop()
{
}

int pit_expired()
{
	uint8_t expired;
	asm volatile("inb $0x61, %0" : "=a"(expired));

	return !(expired & 0x20);
}
