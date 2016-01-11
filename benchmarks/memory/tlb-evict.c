#include "benchmark.h"
#include "debug.h"
#include "harness.h"
#include "mem.h"

#define MEMORY_BENCHMARK_ITERATIONS 2000

__align12;

static size_t virt_start, virt_end;

static void kernel_mmu_init()
{
	mem_init();

	// Map physical memory into a virtual memory region
	const phys_mem_info_t *phys_mem = mem_get_phys_info();
	
	// Identity map the physical memory region
	uintptr_t ptr = phys_mem->phys_mem_start;
	uintptr_t vptr = phys_mem->phys_mem_start;
	
	virt_start = vptr;
	
	size_t pagesize = mem_get_page_size();
	while(ptr < phys_mem->phys_mem_end) {
		mem_create_page_mapping(ptr, vptr);
		ptr += pagesize;
		vptr += pagesize;
	}
	
	virt_end = vptr;
	
	mem_mmu_enable();
	mem_tlb_flush();
}
static void kernel_mmu()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS;
	uint32_t i;

	uintptr_t ptr = virt_start;
	size_t pagesize = mem_get_page_size();
	
	debug_spinner_start(MEMORY_BENCHMARK_ITERATIONS);
	
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		
		*(uint32_t*)ptr = 0x12345678;
		mem_tlb_evict(ptr);
		
		ptr += pagesize;
		if(ptr >= virt_end) ptr = virt_start;
	}
}
static void kernel_mmu_cleanup()
{
	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

static benchmark_t bmark = {
	.name="TLB-Evict",
	.category="Memory",
	.kernel_init=kernel_mmu_init,
	.kernel=kernel_mmu,
	.kernel_cleanup=kernel_mmu_cleanup,
	.iteration_count = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS
};

REG_BENCHMARK(bmark);
