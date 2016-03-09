#include "heap.h"

extern char _HEAP_START;

uintptr_t heap_base, heap_cur;

void heap_init()
{
	heap_base = heap_cur = (uintptr_t)&_HEAP_START;
}

extern void __fast_zero_page(uintptr_t page);

void *heap_alloc_page()
{
	void *page = (void *)heap_cur;
	heap_cur += 0x1000;

	__fast_zero_page(page);
	return page;
}

void heap_free_page(void *p)
{
	//
}
