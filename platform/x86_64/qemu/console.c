#include "console.h"

#define ROWS	25
#define COLUMNS	80

static unsigned int cur_x, cur_y;
static uint16_t *vga = (uint16_t *)0xb8000;

static inline uint16_t *slot_from_xy(unsigned int x, unsigned int y)
{
	if (x >= COLUMNS) return NULL;
	if (y >= ROWS) return NULL;
	
	return &vga[x + (y * COLUMNS)];
}

static void write_reg(char reg, char val)
{
	asm volatile("outb %0, %1\n" :: "a"(reg), "d"((unsigned short)0x3d4));
	asm volatile("outb %0, %1\n" :: "a"(val), "d"((unsigned short)0x3d5));
}

void console_init()
{
	// Disable the cursor
	write_reg(0x0a, 0x20);
	
	console_clear();
}

void console_clear()
{
	for (int i = 0; i < ROWS * COLUMNS; i++) {
		vga[i] = 0;
	}

	cur_x = 0;
	cur_y = 0;
}

static void new_line()
{
	cur_x = 0;

	cur_y++;
	if (cur_y >= ROWS) {
		cur_y = ROWS - 1;
		
		for (int y = 0; y < ROWS - 1; y++) {
			for (int x = 0; x < COLUMNS; x++) {
				*slot_from_xy(x, y) = *slot_from_xy(x, y + 1);
			}
		}
		
		for (int x = 0; x < COLUMNS; x++) {
			*slot_from_xy(x, ROWS - 1) = 0;
		}
	}
}

void console_putc(char ch)
{
	if (ch == '\n') {
		new_line();
	} else if (ch == '\r') {
		cur_x = 0;
	} else {
		uint16_t *slot = slot_from_xy(cur_x, cur_y);
		*slot = 0x0700 | ch;
		
		cur_x++;
		if (cur_x >= COLUMNS) {
			new_line();
		}
	}
}
