#include "arch.h"
#include "mem.h"

void mem_init()
{
	arch_abort();
}

void mem_reset()
{
	arch_abort();
}

void mem_mmu_enable()
{
	arch_abort();
}

void mem_mmu_disable()
{
	arch_abort();
}

void mem_tlb_flush()
{
	arch_abort();
}

void mem_tlb_evict(uintptr_t ptr)
{
	arch_abort();
}

size_t mem_get_page_size()
{
	return 1 << 12;
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	arch_abort();
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	arch_abort();
}
