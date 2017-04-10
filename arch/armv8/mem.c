#include "mem.h"
#include "string.h"
#include "printf.h"
#include "debug.h"

#if !defined(SIMBENCH_DEBUG) && defined(DEBUG_MEM)
#warning "DEBUG_MEM defined, but SIMBENCH_DEBUG not defined"
#endif

#ifdef DEBUG_MEM
#define mem_dprintf(a...) dprintf(a)
#else
#define mem_dprintf(a...) do { } while(0)
#endif


void mem_init()
{	
	
}

void mem_reset()
{

}

void mem_mmu_enable()
{

}

void mem_mmu_disable()
{

}

void mem_tlb_flush()
{

}

void mem_tlb_evict(uintptr_t ptr)
{

}

void mem_cache_flush()
{

}

// Support only 1MB section mappings for now
size_t mem_get_page_size()
{
	return 1 << 20;
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	return 1;
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	return 1;
}

