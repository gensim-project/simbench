#include "arch.h"
#include "benchmark.h"
#include "debug.h"

#include "define.h"

static volatile uint32_t value1;
static volatile uint32_t value2;
static volatile uint32_t value3;
static void __attribute__((noinline)) ALIGN block() 
{
	uint32_t i = value1;
	uint32_t j = value2;
	uint32_t k = value2;
	
	#define STMT(x,y) y += x; x <<= 1; x += y; x ^= 0xf0f0f0f0; x -= y; x >>= 1
		
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	STMT(i,j);
	STMT(j,k);
	STMT(k,i);
	
	value1 = i;
	value2 = j;
	value3 = k;
}

static void ALIGN kernel()
{
	uint64_t total_iterations = BENCHMARK_ITERATIONS * CODEGEN_LB_BENCHMARK_ITERATIONS;
	uint64_t i;
	volatile uint8_t* block_ptr = (volatile uint8_t*)block;
	
	debug_spinner_start(CODEGEN_LB_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i) {
		debug_spinner();
		block();
		*block_ptr = *block_ptr;
		arch_code_flush((size_t)block);
	}
}

static void ALIGN kernel_control()
{
	uint64_t total_iterations = BENCHMARK_ITERATIONS * CODEGEN_LB_BENCHMARK_ITERATIONS;
	uint64_t i;
	volatile uint8_t* block_ptr = (volatile uint8_t*)block;
	uint8_t v;
	debug_spinner_start(CODEGEN_LB_BENCHMARK_ITERATIONS);
	
	for(i=0; i < total_iterations; ++i) {
		debug_spinner();
		block();
		v = *block_ptr;
		arch_code_flush((size_t)block);
	}
}

DEFINE_BENCHMARK(large_blocks) = {
	.name="Large-Blocks",
	.category="Codegen",
	.kernel=kernel,
	.kernel_control=kernel_control,
	.iteration_count = BENCHMARK_ITERATIONS * CODEGEN_LB_BENCHMARK_ITERATIONS
};
