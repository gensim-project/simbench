#include "benchmark.h"
#include "debug.h"

#define NOREORDER 
#define NOINLINE __attribute__((noinline))
#define ALIGN __attribute__((aligned(4096)))

static volatile uint32_t value;

void ipd_fn_10(); 

static void ALIGN kernel()
{
	uint64_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_IPD_BENCHMARK_ITERATIONS;
	uint64_t i;
	
	debug_spinner_start(DISPATCH_IPD_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i)
	{
		debug_spinner();
		ipd_fn_10();
	}
}

DEFINE_BENCHMARK(inter_page_direct) = {
	.name="Inter-Page-Direct",
	.category="Dispatch",
	.kernel=kernel,
	.iteration_count = DISPATCH_IPD_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
