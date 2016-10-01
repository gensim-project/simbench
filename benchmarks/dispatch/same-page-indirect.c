#include "benchmark.h"
#include "debug.h"

#define NOREORDER 
//#define NOREORDER __attribute__((no_reorder))


__align12 NOREORDER;

#define leaf_fn(x) static void NOREORDER spi_fn ## x() {}

#define FN_TABLE_SIZE 16
static benchmark_kernel_t fn_table[];

static void NOREORDER dispatch (int i) {
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

static benchmark_kernel_t fn_table[FN_TABLE_SIZE] = { spi_fn1, spi_fn2, spi_fn3, spi_fn4, spi_fn5, spi_fn6, spi_fn7, spi_fn8, spi_fn9, spi_fn10, spi_fn11, spi_fn12, spi_fn13, spi_fn14, spi_fn15, spi_fn16 };

static void kernel()
{
	uint32_t total_iterations = BENCHMARK_ITERATIONS * DISPATCH_SPI_BENCHMARK_ITERATIONS;
	int i = 0;
	
	debug_spinner_start(DISPATCH_SPI_BENCHMARK_ITERATIONS);
	
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		dispatch(1000);
	}
}

DEFINE_BENCHMARK(intra_page_indirect) = {
	.name="Same-Page-Indirect",
	.category="Dispatch",
	.kernel=kernel,
	.iteration_count = DISPATCH_SPI_BENCHMARK_ITERATIONS * BENCHMARK_ITERATIONS
};
