#include "uart.h"
#include "harness.h"

#define MAX_BENCHMARKS 100

typedef void (*ctor_t)();
extern intptr_t _CTORS_START, _CTORS_END;

static int num_benchmarks;
static benchmark_t *benchmarks[MAX_BENCHMARKS];

void harness_init()
{
	num_benchmarks = 0;
	
	intptr_t *ctor_ptr;

	for(ctor_ptr=&_CTORS_START; ctor_ptr != &_CTORS_END; ctor_ptr++)
	{
		ctor_t ctor = (ctor_t)*ctor_ptr;
		ctor();
	}	
}

void harness_main()
{	
	int i = 0;
	
	uart_puts("\\\\BENCHMARKS BEGIN\r\n");
	
	for(i = 0; i < num_benchmarks; ++i) {
		harness_execute(benchmarks[i]);
	}
	
	uart_puts("\\\\BENCHMARKS END\r\n");
}

void harness_register_benchmark(benchmark_t *benchmark)
{
	benchmarks[num_benchmarks++] = benchmark;
}

void harness_execute(benchmark_t *benchmark)
{
	uart_puts(benchmark->category);
	uart_puts(" - ");
	uart_puts(benchmark->name);
	
	if(benchmark->kernel_init) benchmark->kernel_init();
	
	uart_puts(" [");
	benchmark->kernel();
	uart_puts("]\r\n");
	
	if(benchmark->kernel_cleanup) benchmark->kernel_cleanup();
}
