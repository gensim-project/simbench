#include "benchmark.h"

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
	}
}

static benchmark_t bmark = {
	.name="Memory-Hot",
	.category="Memory",
	.kernel=kernel
};

REG_BENCHMARK(&bmark);
