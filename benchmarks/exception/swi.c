#include "arch.h"
#include "benchmark.h"
#include "debug.h"
#include "mem.h"
#include "platform.h"


__align12;

static volatile uint32_t flag = 0;

static void ALIGN kernel()
{
	uint64_t i, total_iterations = BENCHMARK_ITERATIONS * EXCEPTION_SWI_BENCHMARK_ITERATIONS;
	
	debug_spinner_start(EXCEPTION_SWI_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i) {
		debug_spinner();
		flag = 0;
		platform_trigger_swi();
		while(!flag) ;
	}
}

static void irq_handler()
{
	flag = 1;
	platform_clear_swi();
}

static void kernel_init()
{
	// Need to have MMU on in order to hit the relocated vectors
	mem_init();
	mem_mmu_enable();
	mem_tlb_flush();
	
	arch_irq_install_handler(irq_handler);
	arch_irq_enable();
}
static void kernel_cleanup()
{
	arch_irq_disable();
	
	mem_mmu_disable();
	mem_tlb_flush();
	mem_reset();
}

DEFINE_BENCHMARK(interrupt) = {
	.name="Interrupt",
	.category="Exception",
	.kernel_init=kernel_init,
	.kernel=kernel,
	.kernel_cleanup=kernel_cleanup,
	.iteration_count = EXCEPTION_SWI_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
