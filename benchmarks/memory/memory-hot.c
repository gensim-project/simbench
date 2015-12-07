#include "benchmark.h"
#include "mem.h"

#include <stdint.h>

#define MEMORY_BENCHMARK_ITERATIONS 100000

static void kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS;
	uint32_t i;
	
	//TODO: Enable MMU
	
	static volatile uint32_t value = 0;
	
	for(i = 0; i < total_iterations; ++i) {
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
	}
}

static void kernel_mmu()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS;
	uint32_t i;
	
	mem_init();
	
	static volatile uint32_t value = 0;
	mem_create_page_mapping(&value, &value);
	
	mem_tlb_flush();
	mem_mmu_enable();
	
	for(i = 0; i < total_iterations; ++i) {
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
		value = value;
	}

	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

static benchmark_t bmark = {
	.name="Memory-Hot-NoMMU",
	.category="Memory",
	.kernel=kernel
};

static benchmark_t bmark_mmu = {
	.name="Memory-Hot-MMU",
	.category="Memory",
	.kernel=kernel_mmu
};

REG_BENCHMARK(bmark);
REG_BENCHMARK(bmark_mmu);
