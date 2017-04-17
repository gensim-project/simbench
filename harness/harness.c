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

		if(benchmark->kernel_priv) {
			arch_priv_enter();
		}
		benchmark->kernel_control();
		if(benchmark->kernel_priv) {
			arch_priv_leave();
		}
		
		fprintf(OUTPUT, "}");
	}

	fprintf(OUTPUT, " [");
	if(benchmark->kernel_priv) {
		arch_priv_enter();
	}
	benchmark->kernel();
	if(benchmark->kernel_priv) {
		arch_priv_leave();
	}
	fprintf(OUTPUT, "]\r\n");

	if (benchmark->kernel_cleanup) benchmark->kernel_cleanup();
	
	if(benchmark->kernel_priv) {
		arch_priv_leave();
	}
}

void harness_main()
{	
	const benchmark_t *benchmark;
	
	fprintf(OUTPUT, "\r\n\r\n");
	fprintf(OUTPUT, "\\\\BENCHMARKS BEGIN\r\n");
	
	for (benchmark = &_BENCHMARKS_START[0]; benchmark < _BENCHMARKS_END; benchmark++) {
		harness_execute(benchmark);
	}
	
	fprintf(OUTPUT, "\\\\BENCHMARKS END\r\n");
}
