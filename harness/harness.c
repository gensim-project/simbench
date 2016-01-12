#include "uart.h"
#include "harness.h"
#include "benchmark.h"

extern const benchmark_t _BENCHMARKS_START[], _BENCHMARKS_END[];

void harness_init()
{
}

static void harness_execute(const benchmark_t *benchmark)
{
	uart_puts(benchmark->category);
	uart_puts(" - ");
	uart_puts(benchmark->name);
	
	if(benchmark->kernel_init) benchmark->kernel_init();
	
	uart_puts(" ");
	uart_puthex(benchmark->iteration_count);
	
	if(benchmark->kernel_control) {
		uart_puts(" {");
		benchmark->kernel_control();
		uart_puts("}");
	}
	
	uart_puts(" [");
	benchmark->kernel();
	uart_puts("]\r\n");
	
	if(benchmark->kernel_cleanup) benchmark->kernel_cleanup();
}

void harness_main()
{	
	const benchmark_t *benchmark;
	
	uart_puts("\\\\BENCHMARKS BEGIN\r\n");
	
	for (benchmark = &_BENCHMARKS_START[0]; benchmark < _BENCHMARKS_END; benchmark++) {
		harness_execute(benchmark);
	}
	
	uart_puts("\\\\BENCHMARKS END\r\n");
}
