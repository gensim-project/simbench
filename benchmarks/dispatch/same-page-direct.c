#include "benchmark.h"
#include "debug.h"

#define DISPATCH_ITERATIONS 50000

#define NOREORDER 
#define NOINLINE __attribute__((noinline))

__align12 NOREORDER;

static volatile uint32_t value;

static void NOINLINE fn1() { asm volatile ("":::"memory"); }
static void NOINLINE fn2() { asm volatile ("":::"memory");fn1(); }
static void NOINLINE fn3() { asm volatile ("":::"memory");fn2(); }
static void NOINLINE fn4() { asm volatile ("":::"memory");fn3(); }
static void NOINLINE fn5() { asm volatile ("":::"memory");fn4(); }
static void NOINLINE fn6() { asm volatile ("":::"memory");fn5(); }
static void NOINLINE fn7() { asm volatile ("":::"memory");fn6(); }
static void NOINLINE fn8() { asm volatile ("":::"memory");fn7(); }
static void NOINLINE fn9() { asm volatile ("":::"memory");fn8(); }
static void NOINLINE fn10() { asm volatile ("":::"memory");fn9(); }
static void NOINLINE fn11() { asm volatile ("":::"memory");fn10(); }

static void kernel()
{
	uint64_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_ITERATIONS;
	uint64_t i;
	
	debug_spinner_start(DISPATCH_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i)
	{
		debug_spinner();
		fn11(i);
	}
}

DEFINE_BENCHMARK(intra_page_direct) = {
	.name="Same-Page-Direct",
	.category="Dispatch",
	.kernel=kernel,
	.iteration_count = DISPATCH_ITERATIONS * BENCHMARK_ITERATIONS
};
