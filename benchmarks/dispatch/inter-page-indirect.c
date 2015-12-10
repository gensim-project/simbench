#include "benchmark.h"
#include "harness.h"

#define DISPATCH_ITERATIONS 100

#define leaf_fn(x) static void ALIGN fn ## x() {}

#define FN_TABLE_SIZE 16
static benchmark_kernel_t fn_table[];

static void ALIGN dispatch (int i) {
	while(i--) {
		fn_table[i % FN_TABLE_SIZE]();
	}
}

leaf_fn(1)
leaf_fn(2)
leaf_fn(3)
leaf_fn(4)
leaf_fn(5)
leaf_fn(6)
leaf_fn(7)
leaf_fn(8)
leaf_fn(9)
leaf_fn(10)
leaf_fn(11)
leaf_fn(12)
leaf_fn(13)
leaf_fn(14)
leaf_fn(15)
leaf_fn(16)

static benchmark_kernel_t fn_table[FN_TABLE_SIZE] = { fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fn10, fn11, fn12, fn13, fn14, fn15, fn16 };

static void kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_ITERATIONS;
	int i = 0;
	for(i = 0; i < total_iterations; ++i) {
		dispatch(1000);
	}
}

static benchmark_t bmark = {
	.name="Inter-Page-Indirect",
	.category="Dispatch",
	.kernel=kernel
};
REG_BENCHMARK(bmark);
