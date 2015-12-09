#ifndef HARNESS_H
#define HARNESS_H

#include "benchmark.h"

extern void harness_init();
extern void harness_register_benchmark(benchmark_t *benchmark);
extern void harness_main();
extern void harness_execute(benchmark_t *benchmark);

#endif
