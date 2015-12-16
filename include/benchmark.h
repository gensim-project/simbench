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
	benchmark_kernel_t control_kernel;
	benchmark_kernel_t kernel_cleanup;
} benchmark_t;

#endif
