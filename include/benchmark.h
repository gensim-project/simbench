#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "define.h"

#define NOINLINE __attribute__((noinline))
#define ALIGN __attribute__((aligned(4096)))

#define BENCHMARK_ITERATIONS 1000

typedef void (*benchmark_kernel_t)();

typedef struct {
	const char *name;
	const char *category;
	benchmark_kernel_t kernel_init;
	benchmark_kernel_t kernel;
	benchmark_kernel_t kernel_cleanup;
	benchmark_kernel_t kernel_control;
	uint64_t iteration_count;
} benchmark_t;

#define DEFINE_BENCHMARK(__name) const benchmark_t __attribute__((section(".benchmarks"))) __attribute__((aligned(__alignof__(unsigned long))))  __benchmark_##__name

#endif
