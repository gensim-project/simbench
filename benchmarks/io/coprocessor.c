#include "arch.h"
#include "benchmark.h"
#include "debug.h"
#include "mem.h"
#include "platform.h"


__align12;

static void ALIGN kernel()
{
	uint64_t i, total_iterations = BENCHMARK_ITERATIONS * IO_COPRO_BENCHMARK_ITERATIONS;
	
	debug_spinner_start(IO_COPRO_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i) {
		debug_spinner();
		arch_coprocessor_access();
	}
}

DEFINE_BENCHMARK(coprocessor_access) = {
	.name="Coprocessor",
	.category="IO",
	.kernel=kernel,
	.iteration_count = BENCHMARK_ITERATIONS * IO_COPRO_BENCHMARK_ITERATIONS
};
