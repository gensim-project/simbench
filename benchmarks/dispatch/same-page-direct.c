#include "benchmark.h"
#include "debug.h"


#define NOREORDER 
#define NOINLINE __attribute__((noinline))

__align12 NOREORDER;

static volatile uint32_t value;

static void NOINLINE spd_fn1() { asm volatile ("":::"memory"); }
static void NOINLINE spd_fn2() { asm volatile ("":::"memory");spd_fn1(); }
static void NOINLINE spd_fn3() { asm volatile ("":::"memory");spd_fn2(); }
static void NOINLINE spd_fn4() { asm volatile ("":::"memory");spd_fn3(); }
static void NOINLINE spd_fn5() { asm volatile ("":::"memory");spd_fn4(); }
static void NOINLINE spd_fn6() { asm volatile ("":::"memory");spd_fn5(); }
static void NOINLINE spd_fn7() { asm volatile ("":::"memory");spd_fn6(); }
static void NOINLINE spd_fn8() { asm volatile ("":::"memory");spd_fn7(); }
static void NOINLINE spd_fn9() { asm volatile ("":::"memory");spd_fn8(); }
static void NOINLINE spd_fn10() { asm volatile ("":::"memory");spd_fn9(); }
static void NOINLINE spd_fn11() { asm volatile ("":::"memory");spd_fn10(); }

static void kernel()
{
	uint64_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_SPD_BENCHMARK_ITERATIONS;
	uint64_t i;
	
	debug_spinner_start(DISPATCH_SPD_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i)
	{
		debug_spinner();
		spd_fn11(i);
	}
}

DEFINE_BENCHMARK(intra_page_direct) = {
	.name="Same-Page-Direct",
	.category="Dispatch",
	.kernel=kernel,
	.iteration_count = DISPATCH_SPD_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
