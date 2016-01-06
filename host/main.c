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

int read_benchmark_begin()
{
	if(fgetc(stdin) != '[') return 1;
	return 0;
}

int read_benchmark_end()
{
	while(1) {
		char c = fgetc(stdin);
		
		switch(c) {
			case ']': 
				return 0;
			case '|':
			case '/':
			case '-':
			case '\\':
			case ' ':
			case 0x08:
				continue;
			default: 
				return 1;
		}
	}
}

int read_benchmark() 
{
	char category[128];
	char benchmark[128];
	
	struct timespec start_time, end_time;
	
	if(fscanf(stdin, "%s - %s ", category, benchmark) != 2) return 1;
	
	printf("%s - %s", category, benchmark);
	
	if(read_benchmark_begin()) return 1;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	if(read_benchmark_end()) return 1;
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	
	uint64_t start_ticks = start_time.tv_sec*1000000000 + start_time.tv_nsec;
	uint64_t end_ticks =   end_time.tv_sec*1000000000 + end_time.tv_nsec;
	
	uint64_t ticks_taken = end_ticks - start_ticks;
	double seconds_taken = ticks_taken / 1000000000.0;
	
	printf(" %lf\r\n", seconds_taken);
	
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
