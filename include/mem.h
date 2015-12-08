#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

/* General initialisation and maintenance operations, generally provided by the architecture */
void mem_init();
void mem_reset();

void mem_mmu_enable();
void mem_mmu_disable();

void mem_tlb_flush();
void mem_tlb_evict(void *ptr);

/* Page table manipulation operations */

size_t mem_get_page_size();
int mem_create_page_mapping(size_t phys_addr, size_t virt_addr);

/* Physical memory discovery operations, generally provided by the platform */

typedef struct {
	void *phys_mem_start;
	void *phys_mem_end;
	
	void *next_mem;
	
} phys_mem_info_t;

const phys_mem_info_t *mem_get_phys_info();
const phys_mem_info_t *mem_get_device_info();

#endif
