#ifndef BENCHMARK_H
#define BENCHMARK_H

#define BENCHMARK_ITERATIONS 1000

typedef void (*benchmark_kernel_t)();

typedef struct {
	const char *name;
	const char *category;
	benchmark_kernel_t kernel;
} benchmark_t;

#define REG_BENCHMARK(x) __attribute__((constructor)) static void reg_bmark() { harness_register_benchmark(x); }

#endif
