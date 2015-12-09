#include "string.h"

void * __weak memset(void *s, int c, size_t n)
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

void __weak bzero(void *s, size_t n)
{
	memset(s, 0, n);
}
