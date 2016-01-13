#include "irq.h"
#include "x86.h"
#include "arch.h"
#include "debug.h"
#include "printf.h"
#include "heap.h"

struct IDT {
	uint16_t off_low;
	uint16_t sel;
	uint8_t zero0;
	uint8_t type;
	uint16_t off_mid;
	uint32_t off_high;
	uint32_t zero1;
} __attribute__((packed));

typedef void (*trap_fn_t)(struct mcontext *);

extern void trap_unknown(struct mcontext *);
extern void trap_unknown_arg(struct mcontext *);
extern void trap_page_fault(struct mcontext *);
extern void trap_syscall(struct mcontext *);
extern void trap_irq(struct mcontext *);
extern void trap_undef(struct mcontext *);

syscall_handler_fn_t syscall_handler_fn;
page_fault_handler_fn_t page_fault_handler_fn;
irq_handler_fn_t irq_handler_fn;
undef_handler_fn_t undef_handler_fn;

static void default_syscall_handler(struct mcontext *ctx)
{
	fprintf(ERROR, "x86: unhandled syscall: rip=%p\n", ctx->rip);
	arch_abort();
}

static void default_page_fault_handler(struct mcontext *mcontext, uint64_t va)
{
	fprintf(ERROR, "x86: unhandled page-fault: code=%lx, rip=%p, va=%p\n", mcontext->extra, mcontext->rip, va);
	arch_abort();
}

static void default_irq_handler(struct mcontext *mcontext)
{
	fprintf(ERROR, "x86: unhandled irq: rip=%p\n", mcontext->rip);
	arch_abort();
}

static void default_undef_handler(struct mcontext *mcontext)
{
	fprintf(ERROR, "x86: undefined instruction: rip=%p\n", mcontext->rip);
	arch_abort();
}

static void set_idt(struct IDT *idt, trap_fn_t fn, int allow_user)
{
	idt->zero0 = 0;
	idt->zero1 = 0;

	idt->off_low = ((uint64_t)fn) & 0xffff;
	idt->off_mid = (((uint64_t)fn) >> 16) & 0xffff;
	idt->off_high = (((uint64_t)fn) >> 32);

	idt->sel = 0x8;
	idt->type = 0x8e | (allow_user ? 0x60 : 0);
}

static void lapic_write(uint32_t reg, uint32_t value)
{
	LAPIC[reg >> 2] = value;
	LAPIC[1];
}

static uint32_t lapic_read(uint32_t reg)
{
	return LAPIC[reg >> 2];
}

void lapic_acknowledge_irq()
{
	lapic_write(EOI, 0);
}

void lapic_issue_interrupt(int irq)
{
	lapic_write(ICRHI, 0);
	lapic_write(ICRLO, irq & 0xff);
}

static void lapic_init()
{
	lapic_write(SVR, 0x1ff);

	lapic_write(LINT0, MASKED);
	lapic_write(LINT1, MASKED);
	lapic_write(LERROR, MASKED);

	lapic_write(ESR, 0);
	lapic_write(ESR, 0);

	lapic_write(EOI, 0);

	lapic_write(ICRHI, 0);
	lapic_write(ICRLO, BCAST | INIT | LEVEL);

	while (lapic_read(ICRLO) & DELIVS);

	lapic_write(TPR, 0);
}

void irq_init()
{
	int i;
	
	struct {
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed)) IDTR;

	IDTR.limit = sizeof(struct IDT) * 0x100;
	IDTR.base = (uint64_t)heap_alloc_page();

	struct IDT *idt = (struct IDT *)IDTR.base;

	// Initialise the table with unknowns
	for (i = 0; i < 0x100; i++) {
		set_idt(&idt[i], trap_unknown, 0);
	}

	set_idt(&idt[0x06], trap_undef, 0);
	set_idt(&idt[0x08], trap_unknown_arg, 0);
	set_idt(&idt[0x08], trap_unknown_arg, 0);
	set_idt(&idt[0x0a], trap_unknown_arg, 0);
	set_idt(&idt[0x0b], trap_unknown_arg, 0);
	set_idt(&idt[0x0c], trap_unknown_arg, 0);
	set_idt(&idt[0x0d], trap_unknown_arg, 0);
	set_idt(&idt[0x0e], trap_page_fault, 0);
	set_idt(&idt[0x11], trap_unknown_arg, 0);
	set_idt(&idt[0x1e], trap_unknown_arg, 0);
	set_idt(&idt[0x30], trap_irq, 0);
	set_idt(&idt[0x80], trap_syscall, 1);

	asm volatile("lidt %0\n" :: "m"(IDTR));
	
	lapic_init();

	irq_reset_syscall_handler();
	irq_reset_page_fault_handler();
	irq_reset_irq_handler();
	irq_reset_undef_handler();
}

void irq_install_syscall_handler(syscall_handler_fn_t handler_fn)
{
	syscall_handler_fn = handler_fn;
}

void irq_install_page_fault_handler(page_fault_handler_fn_t handler_fn)
{
	page_fault_handler_fn = handler_fn;
}

void irq_install_irq_handler(irq_handler_fn_t handler_fn)
{
	irq_handler_fn = handler_fn;
}

void irq_install_undef_handler(undef_handler_fn_t handler_fn)
{
	undef_handler_fn = handler_fn;
}

void irq_reset_syscall_handler()
{
	syscall_handler_fn = default_syscall_handler;
}

void irq_reset_page_fault_handler()
{
	page_fault_handler_fn = default_page_fault_handler;
}

void irq_reset_irq_handler()
{
	irq_handler_fn = default_irq_handler;
}

void irq_reset_undef_handler()
{
	undef_handler_fn = default_undef_handler;
}

void handle_trap_unknown(struct mcontext *mcontext)
{
	fprintf(ERROR, "unknown exception from rip=%lx\n", mcontext->rip);
	arch_abort();
}

void handle_trap_unknown_arg(struct mcontext *mcontext, uint64_t val)
{
	fprintf(ERROR, "unknown exception from rip=%lx, val=%lx\n", mcontext->rip, val);
	arch_abort();
}
