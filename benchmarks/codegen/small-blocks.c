#include "arch.h"
#include "debug.h"
#include "benchmark.h"
#include "harness.h"

#define CODEGEN_ITERATIONS 100

static benchmark_kernel_t volatile fn_table[];

// For each function, indirectly call the next, in order to avoid direct chaining during translation
#define DOFN(x) static void NOINLINE fn##x() { fn_table[x](); }
#define LASTFN(x) static void NOINLINE fn##x() { }
#include "small-blocks-fns.h"
#undef DOFN
#undef LASTFN
static benchmark_kernel_t volatile fn_table[] = {
#define DOFN(x) fn##x,
#define LASTFN(x) fn##x
#include "small-blocks-fns.h"
#undef DOFN
#undef LASTFN
	};

// Make the code region dirty, so that it will be retranslated next time it is executed
static void ALIGN copy_code()
{
	int i = 0;
	for(i=0; i < sizeof(fn_table)/sizeof(*fn_table); ++i) {
		volatile uint8_t* fn_head = (volatile uint8_t*)fn_table[i];
		*fn_head = *fn_head;
		arch_code_flush((size_t)fn_head);
	}
}

// Make the code region dirty, so that it will be retranslated next time it is executed
static void ALIGN copy_code_control()
{
	int i = 0;
	volatile uint8_t v;
	for(i=0; i < sizeof(fn_table)/sizeof(*fn_table); ++i) {
		volatile uint8_t* fn_head = (volatile uint8_t*)fn_table[i];
		v = *fn_head;
		arch_code_flush((size_t)fn_head);
	}
}

// Branch to the first entry in the code region
static void run_code()
{
	fn1();
}

static void kernel_init()
{
	int i;
	for(i=0; i < sizeof(fn_table)/sizeof(*fn_table); ++i) {
		fn_table[i] = fn_table[i];
	}
}

static void ALIGN kernel()
{
	uint32_t i;
	uint32_t total_iterations = BENCHMARK_ITERATIONS * CODEGEN_ITERATIONS;
	
	debug_spinner_start(CODEGEN_ITERATIONS);
	
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		run_code();
		copy_code();
	}
}

static void ALIGN kernel_control()
{
	uint32_t i;
	uint32_t total_iterations = BENCHMARK_ITERATIONS * CODEGEN_ITERATIONS;
	
	debug_spinner_start(CODEGEN_ITERATIONS);
	
	for(i = 0; i < total_iterations; ++i) {
		debug_spinner();
		run_code();
		copy_code_control();
	}
}

static benchmark_t bmark = {
	.name="Small-Blocks",
	.category="Codegen",
	.kernel_init=kernel_init,
	.kernel=kernel,
	.kernel_control=kernel_control,
	.iteration_count = BENCHMARK_ITERATIONS * CODEGEN_ITERATIONS
};
REG_BENCHMARK(bmark);
