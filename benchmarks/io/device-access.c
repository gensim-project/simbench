#include "arch.h"
#include "benchmark.h"
#include "debug.h"
#include "mem.h"
#include "platform.h"

#define IO_BENCHMARK_ITERATIONS 500000

__align12;

static void ALIGN kernel()
{
	uint64_t i, total_iterations = BENCHMARK_ITERATIONS * IO_BENCHMARK_ITERATIONS;
	
	debug_spinner_start(IO_BENCHMARK_ITERATIONS);
	
	volatile uint32_t *ptr = platform_get_null_devptr();
	uint32_t val = platform_get_null_devval();
	
	for(i=0; i < total_iterations; ++i) {
		*ptr = val;
	}
}

DEFINE_BENCHMARK(device_access) = {
	.name="Device-Access",
	.category="IO",
	.kernel=kernel,
	.iteration_count = BENCHMARK_ITERATIONS * IO_BENCHMARK_ITERATIONS
};
