#include "harness.h"
#include "benchmark.h"
#include "printf.h"

extern const benchmark_t _BENCHMARKS_START[], _BENCHMARKS_END[];

void harness_init()
{
}

static void harness_execute(const benchmark_t *benchmark)
{
	fprintf(OUTPUT, "%s - %s", benchmark->category, benchmark->name);
	
	if (benchmark->kernel_init) benchmark->kernel_init();
	
	fprintf(OUTPUT, " %x", benchmark->iteration_count);
	
	if (benchmark->kernel_control) {
		fprintf(OUTPUT, " {");
		benchmark->kernel_control();
		fprintf(OUTPUT, "}");
	}
	
	fprintf(OUTPUT, " [");
	benchmark->kernel();
	fprintf(OUTPUT, "]\r\n");
	
	if (benchmark->kernel_cleanup) benchmark->kernel_cleanup();
}

void harness_main()
{	
	const benchmark_t *benchmark;
	
	fprintf(OUTPUT, "\\\\BENCHMARKS BEGIN\r\n");
	
	for (benchmark = &_BENCHMARKS_START[0]; benchmark < _BENCHMARKS_END; benchmark++) {
		harness_execute(benchmark);
	}
	
	fprintf(OUTPUT, "\\\\BENCHMARKS END\r\n");
}
