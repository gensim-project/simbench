#ifndef HEAP_H
#define HEAP_H

#include "define.h"

extern void heap_init();
extern void *heap_alloc_page();
extern void heap_free_page(void *);

#endif /* HEAP_H */

