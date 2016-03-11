#include "heap.h"
#include "arch.h"
#include "printf.h"

extern char _HEAP_START, _HEAP_SIZE;

uintptr_t heap_base, heap_cur, heap_end;
size_t heap_size;

void heap_init()
{
	heap_base = heap_cur = (uintptr_t)&_HEAP_START;
	heap_size = (size_t)&_HEAP_SIZE;
	heap_end = heap_base + heap_size;
}

extern void __fast_zero_page(void *page);

void *heap_alloc_page()
{
	void *page = (void *)heap_cur;

	if (heap_cur >= heap_end) {
		fprintf(ERROR, "out of memory\n");
		arch_abort();
	}

	heap_cur += 0x1000;

	__fast_zero_page(page);
	return page;
}

void heap_free_page(void *p)
{
	//
}
