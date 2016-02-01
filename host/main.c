#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <time.h>

#define STR_BENCHMARK_BEGIN "\\\\BENCHMARKS BEGIN\r\n"

int read_start()
{
	char *lineptr = NULL;
	size_t lineptrsize = 0;
	while(1) {
		if(getline(&lineptr, &lineptrsize, stdin) < 0) return 1;
		if(!strcmp(STR_BENCHMARK_BEGIN, lineptr)) {
			break;
		}
	}
	
	free(lineptr);
	printf("%s", STR_BENCHMARK_BEGIN);
	
	return 0;
}

static char read_timing_start_char()
{
	while(1) {
		char c = fgetc(stdin);
		
		switch(c) {
			case '{': 
			case '[': 
				return c;
			case '|':
			case '/':
			case '-':
			case '\\':
			case ' ':
			case 0x08:
				continue;
			default: 
				return 0;
		}
	}
}

static char read_timing_end_char(char target)
{
	while(1) {
		char c = fgetc(stdin);
		if(c == target) return target;
		
		switch(c) {
			case '|':
			case '/':
			case '-':
			case '\\':
			case ' ':
			case 0x08:
				continue;
			default: 
				return 0;
		}
	}
}

typedef struct {
	uint64_t control_ticks;
	uint64_t benchmark_ticks;
} benchmark_times_t;

static uint64_t get_ticks_difference(struct timespec start, struct timespec end)
{
	uint64_t start_ticks = (uint64_t)start.tv_sec*(uint64_t)1000000000 + (uint64_t)start.tv_nsec;
	uint64_t end_ticks =   (uint64_t)end.tv_sec*(uint64_t)1000000000 +(uint64_t) end.tv_nsec;
	
	uint64_t ticks_taken = end_ticks - start_ticks;
	
	return ticks_taken;
}

static benchmark_times_t time_benchmarks()
{
	benchmark_times_t times;
	struct timespec start_time, end_time;
	
	char time_start_char = read_timing_start_char();
	if(time_start_char == '{') {
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		read_timing_end_char('}');
		clock_gettime(CLOCK_MONOTONIC, &end_time);
		
		times.control_ticks = get_ticks_difference(start_time, end_time);
		
		time_start_char = read_timing_start_char();
	} else {
		times.control_ticks = 0;
	}
	
	if(time_start_char == '[') {
		clock_gettime(CLOCK_MONOTONIC, &start_time);
		read_timing_end_char(']');
		clock_gettime(CLOCK_MONOTONIC, &end_time);
		
		times.benchmark_ticks = get_ticks_difference(start_time, end_time);		
	}
	
	return times;
}

int read_benchmark() 
{
	char category[128];
	char benchmark[128];
	uint64_t iterations;
	
	struct timespec start_time, end_time;
	
	if(fscanf(stdin, "%s - %s %lx ", category, benchmark, &iterations) != 3) return 1;
	
	printf("%s - %s %lx ", category, benchmark, iterations);
	
	benchmark_times_t times = time_benchmarks();
	
	
	double control_seconds = times.control_ticks / 1000000000.0;
	double benchmarks_seconds = times.benchmark_ticks / 1000000000.0;
	if(control_seconds) printf(" %lf", control_seconds);
	
	printf(" %lf\r\n", benchmarks_seconds);
	
	return 0;
}

int main(int argc, char **argv)
{
	if(!read_start()) {
		while(!read_benchmark()) ;
	}
	
	printf("Done.\n");

	return 0;
}
