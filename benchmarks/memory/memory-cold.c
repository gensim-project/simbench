#include "benchmark.h"
#include "harness.h"

#define MEMORY_BENCHMARK_ITERATIONS 2

static void ALIGN kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_BENCHMARK_ITERATIONS;

	//TODO: Actually map virtual memory
	volatile uint32_t *mem_region_start = (uint32_t*)0x40000000;
	volatile uint32_t *mem_region_end =   (uint32_t*)0x50000000;
	
	uint32_t page_size = 4096;
	
	int it;
	for(it = 0; it < total_iterations; ++it) {
		volatile uint32_t *i;
		for(i = mem_region_start; i < mem_region_end; i += page_size / sizeof(*i)) {
			*i = (intptr_t)i;
		}
	}
}

static benchmark_t bmark = {
	.name="Memory-Cold",
	.category="Memory",
	.kernel=kernel
};

REG_BENCHMARK(bmark);
