#include "benchmark.h"
#include "debug.h"
#include "harness.h"
#include "mem.h"

#define MEMORY_BENCHMARK_ITERATIONS 20000

static void ALIGN kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS;

	const phys_mem_info_t *phys_mem = mem_get_phys_info();
	volatile uint32_t *mem_region_start = (uint32_t*)phys_mem->phys_mem_start;
	volatile uint32_t *mem_region_end =   (uint32_t*)phys_mem->phys_mem_end;
	
	uint32_t page_size = 4096;
	
	debug_spinner_start(MEMORY_BENCHMARK_ITERATIONS);
	
	int it;
	volatile uint8_t *ptr = (uint8_t*)mem_region_start;
	for(it = 0; it < total_iterations; ++it) {
		debug_spinner();
		*ptr;
		ptr += page_size;
		if(ptr >= mem_region_end) ptr = mem_region_start;
	}
}

static void kernel_mmu_init()
{
	mem_init();
	
	const phys_mem_info_t *phys_mem = mem_get_phys_info();
	uintptr_t mem_region_start = phys_mem->phys_mem_start;
	uintptr_t mem_region_end =   phys_mem->phys_mem_end;
	uint32_t page_size = mem_get_page_size();
	while(mem_region_start < mem_region_end) {
		mem_create_page_mapping(mem_region_start, mem_region_start);
		mem_region_start += page_size;
	}
		
	mem_mmu_enable();
	mem_tlb_flush();
}
static void kernel_mmu_cleanup()
{
	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

static benchmark_t bmark = {
	.name="Memory-Cold-NoMMU",
	.category="Memory",
	.kernel=kernel
};

static benchmark_t bmark_mmu = {
	.name="Memory-Cold-MMU",
	.category="Memory",
	.kernel_init=kernel_mmu_init,
	.kernel=kernel,
	.kernel_cleanup=kernel_mmu_cleanup,
};

REG_BENCHMARK(bmark);
REG_BENCHMARK(bmark_mmu);
