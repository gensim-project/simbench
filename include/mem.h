#ifndef MEM_H
#define MEM_H

#include "define.h"

/* General initialisation and maintenance operations, generally provided by the architecture */
extern void mem_init();
extern void mem_reset();

extern void mem_mmu_enable();
extern void mem_mmu_disable();

extern void mem_tlb_flush();
extern void mem_tlb_evict(uintptr_t ptr);

/* Cache flushing operations */

extern void mem_cache_flush();

/* Page table manipulation operations */

extern size_t mem_get_page_size();
extern int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr);
extern int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr);

/* Physical memory discovery operations, generally provided by the platform */

typedef struct {
	uintptr_t phys_mem_start;
	uintptr_t phys_mem_end;
	
	void *next_mem;
	
} phys_mem_info_t;

extern const phys_mem_info_t *mem_get_phys_info();
extern const phys_mem_info_t *mem_get_device_info();

#endif
