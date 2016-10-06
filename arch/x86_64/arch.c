#include "arch.h"
#include "platform.h"
#include "harness.h"
#include "debug.h"
#include "printf.h"
#include "irq.h"
#include "heap.h"

int use_framebuffer;

extern void early_mem_init();

void arch_init()
{
	heap_init();

	early_mem_init();
	irq_init();
}

void arch_abort()
{
	fprintf(ERROR, "x86-64: abort!\n");
	for (;;) asm volatile("hlt\n");
}

void arch_code_flush(size_t address)
{

}

uint32_t arch_nonpriviliged_read(volatile uint32_t *ptr)
{
	return *ptr;
}

static void parse_cmdline(uintptr_t cmdline_ptr)
{
	const char *cmdline = (const char *)cmdline_ptr;
	while (*cmdline) {
		switch (*cmdline++) {
		case 'f':
			switch (*cmdline++) {
			case 'b':
				switch (*cmdline++) {
				case ' ':
				case '\0':
					use_framebuffer = 1;
					break;
				}
			}
			break;
		}
	}
}

extern char _HEAP_START;
void arch_start(unsigned int magic, void *mb_info)
{
	uint64_t rsp;
	asm volatile("mov %%rsp, %0\n" : "=r"(rsp));

	parse_cmdline(0x7000);

	arch_init();
	platform_init();

	dprintf("x86-64: arch_start(magic=%08x, mb_info=%p), heap-start=%p, stack=%p\n", magic, mb_info, &_HEAP_START, rsp);

	harness_init();
	harness_main();
	platform_shutdown();
}

void arch_coprocessor_access()
{
	asm volatile("outb %0, $0xf1\n" :: "a"((char)0));
}

void arch_irq_enable()
{
	asm volatile("sti\n");
}

void arch_irq_disable()
{
	asm volatile("cli\n");
}

static irq_handler_t benchmark_irq_handler;

static void arch_irq_handler(struct mcontext *ctx)
{
	if (benchmark_irq_handler) benchmark_irq_handler();
}

void arch_irq_install_handler(irq_handler_t handler)
{
	benchmark_irq_handler = handler;
	
	if (handler)
		irq_install_irq_handler(arch_irq_handler);
	else
		irq_reset_irq_handler();
}
