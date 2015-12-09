#include "string.h"

void * __weak memcpy(void *dest, const void *src, size_t n)
{
	uint8_t *d = (uint8_t*)dest;
	uint8_t *s = (uint8_t*)src;
	
	while(n--) {
		*d++ = *s++;
	}
	
	return dest;
}
