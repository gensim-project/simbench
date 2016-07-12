#include "benchmark.h"
#include "debug.h"

#define ALIGN __attribute__((aligned(4096)))
#define leaf_fn(x) static void ALIGN ipi_fn ## x() {}

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

static benchmark_kernel_t fn_table[FN_TABLE_SIZE] = { ipi_fn1, ipi_fn2, ipi_fn3, ipi_fn4, ipi_fn5, ipi_fn6, ipi_fn7, ipi_fn8, ipi_fn9, ipi_fn10, ipi_fn11, ipi_fn12, ipi_fn13, ipi_fn14, ipi_fn15, ipi_fn16 };

static void kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_IPI_BENCHMARK_ITERATIONS;
	int i = 0;
	
	debug_spinner_start(DISPATCH_IPI_BENCHMARK_ITERATIONS);
	
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		dispatch(1000);
	}
}

DEFINE_BENCHMARK(inter_page_indirect) = {
	.name="Inter-Page-Indirect",
	.category="Dispatch",
	.kernel=kernel,
	.iteration_count = DISPATCH_IPI_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
