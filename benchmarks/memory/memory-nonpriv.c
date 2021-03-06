#include "benchmark.h"
#include "debug.h"
#include "mem.h"
#include "arch.h"


static volatile uint32_t value = 0;

static void ALIGN kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * MEMORY_NONPRIV_BENCHMARK_ITERATIONS;
	uint32_t i;
		
	debug_spinner_start(MEMORY_NONPRIV_BENCHMARK_ITERATIONS);
		
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		arch_nonpriviliged_read(&value);
	}
}

static void kernel_mmu_init()
{
	mem_init();
	mem_create_page_mapping_data((uintptr_t)&value, (uintptr_t)&value);
	
	mem_mmu_enable();
	mem_tlb_flush();
}
static void kernel_mmu_cleanup()
{
	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

DEFINE_BENCHMARK(memory_nonpriv) = {
	.name="Memory-NonPriv",
	.category="Memory",
	.kernel_init=kernel_mmu_init,
	.kernel=kernel,
	.kernel_cleanup=kernel_mmu_cleanup,
	.iteration_count = BENCHMARK_ITERATIONS * MEMORY_NONPRIV_BENCHMARK_ITERATIONS,
	.kernel_priv=1
};
