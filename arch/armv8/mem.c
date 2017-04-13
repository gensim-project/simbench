#include "mem.h"
#include "string.h"
#include "printf.h"
#include "debug.h"
#include "syscall.h"
#include "heap.h"
#include "txln_table.h"

#define DEBUG_MEM
#define SIMBENCH_DEBUG

#if !defined(SIMBENCH_DEBUG) && defined(DEBUG_MEM)
#warning "DEBUG_MEM defined, but SIMBENCH_DEBUG not defined"
#endif

#ifdef DEBUG_MEM
#define mem_dprintf(a...) dprintf(a)
#else
#define mem_dprintf(a...) do { } while(0)
#endif

extern char _TEXT_START, _TEXT_END, _DATA_START, _DATA_END;

static int mem_inited = 0;

static int mem_create_region_id_mapping(uintptr_t region_start, uintptr_t region_end)
{
	uintptr_t i;
	size_t page_size = mem_get_page_size();
	
	region_start &= ~(page_size-1);
	
	for(i = region_start; i <= region_end; i += page_size) {
		if(mem_create_page_mapping(i, i)) return 1;
	}
	return 0;
}

static int mem_create_region_device_mapping(uintptr_t region_start, uintptr_t region_end)
{
	uintptr_t i;
	size_t page_size = mem_get_page_size();
	
	region_start &= ~(page_size-1);
	
	for(i = region_start; i <= region_end; i += page_size) {
		if(mem_create_page_mapping_device(i, i)) {
			mem_dprintf("Failed to map a device at %p!\r\n", i);
			return 1;
		}
	}
	return 0;
}


void mem_init()
{	
	armv8_syscall(SYSCALL_MEM_INIT);
}

int syscall_mem_init() {
	// Set TCR_EL1.T0SZ = 16 to give us a 48 bit virtual address space
	uint32_t t0sz = 64 - VA_BITS;
	uint32_t t1sz = 16; // just use the minimum value
	
	// clear sctlr.wxn
	uint32_t sctlr;
	asm volatile ("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr &= ~(1 << 19);
	asm volatile ("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	// set ttbr to the page table address
	
	mem_dprintf("Using %p as ttbr\n", get_txln_table());
	asm volatile ("msr TTBR0_EL1, %0" :: "r"(get_txln_table()));
	
	// fill in the TCR
	uint64_t TCR = 0;
	TCR |= t0sz << 0;
	TCR |= t1sz << 16;
	mem_dprintf("Using %lx as tcr\n", TCR);
	asm volatile("msr TCR_EL1, %0" :: "r"(TCR));
	
	// mark all l0 page table entries as not present (but leave 
	// addresses of subsequent tables)
	clear_txln_table();
	
	// map simbench memory
	
	mem_dprintf("Mapping text\r\n");
	mem_create_region_id_mapping((uintptr_t)&_TEXT_START, (uintptr_t)&_TEXT_END);
	
	mem_dprintf("Mapping data\r\n");
	mem_create_region_id_mapping((uintptr_t)&_DATA_START, (uintptr_t)&_DATA_END);
	
	const phys_mem_info_t *dev_info = mem_get_device_info();
	mem_dprintf("Mapping devices\r\n");
	while(dev_info) {
		mem_create_region_device_mapping(dev_info->phys_mem_start, dev_info->phys_mem_end);
		dev_info = dev_info->next_mem;
	}
	
	// done.
	return 0;
}

int syscall_mem_mmu_enable() {
	uint32_t sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr |= 1;
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	return 0;
}

int syscall_mem_mmu_disable() {
	uint32_t sctlr;
	asm volatile("mrs %0, SCTLR_EL1" : "=r"(sctlr));
	sctlr &= ~1;
	asm volatile("msr SCTLR_EL1, %0" :: "r"(sctlr));
	
	return 0;
}

void mem_reset()
{
	mem_init();
}

void mem_mmu_enable()
{
	armv8_syscall(SYSCALL_MMU_ENABLE);
}

void mem_mmu_disable()
{
	armv8_syscall(SYSCALL_MMU_DISABLE);
}

void mem_tlb_flush()
{

}

void mem_tlb_evict(uintptr_t ptr)
{

}

void mem_cache_flush()
{
	armv8_syscall(SYSCALL_FLUSH_ICACHE);
	armv8_syscall(SYSCALL_FLUSH_DCACHE);
}

// Support 4k mappings
size_t mem_get_page_size()
{
	return 1 << 12;
}

