#include "benchmark.h"
#include "debug.h"
#include "harness.h"

#define DISPATCH_ITERATIONS 50000

#define NOREORDER 
#define NOINLINE __attribute__((noinline))

__align12 NOREORDER;

static volatile uint32_t value;

static void NOINLINE fn1()
{
	asm volatile ("");
	value = 1;
}

static void NOINLINE fn2()
{
	asm volatile ("");
	value = 2;
}

static void NOINLINE fn3(int x)
{
	asm volatile ("");
	if(x & 1) fn1();
	else fn2();
}

static void NOINLINE fn4()
{
	asm volatile ("");
	fn1();
}

static void NOINLINE fn5(int x)
{
	asm volatile ("");
	if(x & 1) fn3(x >> 1);
	else fn4();
}

static void NOINLINE fn6(int x)
{
	asm volatile ("");
	if(x & 1) fn3(x >> 1);
	else fn1();
}

static void NOINLINE fn7(int x)
{
	asm volatile ("");
	if(x & 1) fn6(x >> 1);
	else fn5(x >> 1);
}


static void NOINLINE fn8(int x)
{
	asm volatile ("");
	if(x & 1) fn1();
	else fn3(x >> 1);
}


static void NOINLINE fn9(int x)
{
	asm volatile ("");
	if(x & 1) fn8(x >> 1);
	else fn5(x >> 1);
}


static void NOINLINE fn10(int x)
{
	asm volatile ("");
	if(x & 1) fn8(x >> 1);
	else fn7(x >> 1);
}


static void NOINLINE fn11(int x)
{
	asm volatile ("");
	if(x & 1) fn9(x >> 1);
	else fn10(x >> 1);
}

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

static benchmark_t bmark = {
	.name="Same-Page-Direct",
	.category="Dispatch",
	.kernel=kernel
};
REG_BENCHMARK(bmark);
