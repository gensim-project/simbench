#ifndef HARNESS_H
#define HARNESS_H

#include "benchmark.h"

void harness_init();
void harness_register_benchmark(benchmark_t *benchmark);
void harness_execute();

#endif
