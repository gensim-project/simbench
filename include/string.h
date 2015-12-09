#ifndef STRING_H
#define STRING_H

#include "define.h"

extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern void bzero(void *s, int c);

#endif /* STRING_H */

