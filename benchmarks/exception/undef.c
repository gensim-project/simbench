#include "arch.h"
#include "benchmark.h"
#include "debug.h"
#include "mem.h"
#include "printf.h"

#define EXCEPTION_BENCHMARK_ITERATIONS 2500

__align12;

static void ALIGN kernel()
{
	uint64_t i, total_iterations = BENCHMARK_ITERATIONS * EXCEPTION_BENCHMARK_ITERATIONS;
	
	debug_spinner_start(EXCEPTION_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i) {
		debug_spinner();
		arch_undefined_instruction();
	}
}

static void kernel_init()
{
	mem_init();
	mem_mmu_enable();
	mem_tlb_flush();

	arch_undef_install_skip();
}
static void kernel_cleanup()
{
	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

DEFINE_BENCHMARK(undef) = {
	.name="Undef-Instruction",
	.category="Exception",
	.kernel_init=kernel_init,
	.kernel=kernel,
	.kernel_cleanup=kernel_cleanup,
	.iteration_count = EXCEPTION_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
