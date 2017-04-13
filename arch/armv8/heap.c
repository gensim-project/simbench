#include "arch.h"
#include "heap.h"
#include "printf.h"

#define PAGE_SIZE 4096

extern void *_HEAP_START;
extern void *_HEAP_END;
static char *heap_ptr;

void armv8_heap_init() {
	heap_ptr = (char*)&_HEAP_START;
}

void *armv8_heap_get_page() {	
	void *page_ptr = heap_ptr;
	heap_ptr += PAGE_SIZE;
	
	if((void*)heap_ptr > (void*)&_HEAP_END) {
		fprintf(ERROR, "ARMv8: OUT OF HEAP\n");
		arch_abort();
	}
	
	return page_ptr;
}
