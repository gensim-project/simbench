#include <stddef.h>

void *memset(void *s, int c, size_t n)
{
	volatile char *_s = (char*)s;
	char b = c & 0xff;
	while(n > 0)
	{
		*_s = b;
		_s++;
		n--;
	}
	return s;
}
