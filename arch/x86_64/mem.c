#include "arch.h"
#include "mem.h"
#include "x86.h"
#include "printf.h"
#include "heap.h"

page_fault_handler_fn_t page_fault_handler_fn;

static uintptr_t initial_pagetables;
static uintptr_t runtime_pagetables;

extern char _TEXT_START, _TEXT_END, _DATA_START, _DATA_END, _HEAP_START, _HEAP_SIZE;

#define PAGE_ADDR(a) (uintptr_t)&(a)
#define NR_PAGES(a, b) ((((uintptr_t)&(b) - (uintptr_t)&(a)) / 0x1000) + 1)

void mem_install_page_fault_handler(page_fault_handler_fn_t handler_fn)
{
	page_fault_handler_fn = handler_fn;
}

static void map_page_ranges(uintptr_t phys_addr, uintptr_t virt_addr, int nr_pages)
{
	int i;
	
	for (i = 0; i < nr_pages; i++) {
		mem_create_page_mapping(phys_addr, virt_addr);
		
		phys_addr += 0x1000;
		virt_addr += 0x1000;
	}
}

static void prepare_runtime_pagetables()
{
	// Ensure the initial page tables are mapped.
	map_page_ranges(initial_pagetables, initial_pagetables, 2);
	
	// Map the .text section
	map_page_ranges(PAGE_ADDR(_TEXT_START), PAGE_ADDR(_TEXT_START), NR_PAGES(_TEXT_START, _TEXT_END));

	// Map the .data section
	map_page_ranges(PAGE_ADDR(_DATA_START), PAGE_ADDR(_DATA_START), NR_PAGES(_DATA_START, _DATA_END));
	
	// Map the heap
	map_page_ranges(PAGE_ADDR(_HEAP_START), PAGE_ADDR(_HEAP_START), ((uintptr_t)&_HEAP_SIZE) >> 12);
}

static void default_page_fault_handler(struct mcontext *mcontext, uint64_t va)
{
	printf("unhandled page-fault: code=%lx, rip=%p, va=%p\n", mcontext->extra, mcontext->rip, va);
	arch_abort();
}

static int mem_inited;

void mem_init()
{
	if (mem_inited) return;
	
	mem_inited = 1;
	mem_install_page_fault_handler(default_page_fault_handler);
	
	initial_pagetables = (uintptr_t)0xc000;
	runtime_pagetables = (uintptr_t)heap_alloc_page();

	prepare_runtime_pagetables();
}

void mem_reset()
{
	mem_install_page_fault_handler(default_page_fault_handler);
}

void mem_mmu_enable()
{
	asm volatile("mov %0, %%cr3\n" :: "r"(runtime_pagetables));
}

void mem_mmu_disable()
{
	asm volatile("mov %0, %%cr3\n" :: "r"(initial_pagetables));
}

void mem_tlb_flush()
{
	asm volatile(
		"mov %cr3, %rax\n"
		"mov %rax, %cr3\n");
}

void mem_tlb_evict(uintptr_t ptr)
{
	asm volatile("invlpg (%0)\n" :: "r"(ptr));
}

size_t mem_get_page_size()
{
	return 1 << 12;
}

typedef struct {
	int pml, pdp, pd, pt;
} table_indicies;

typedef uint64_t pte;

static inline void set_base_address(pte *pte, uint64_t base_addr)
{
	*pte = (base_addr & ~0xfff) | ((*pte) & 0xfff);
}

static inline uint64_t get_base_address(const pte *pte)
{
	return (*pte) & ~0xfff;
}

static inline void set_flags(pte *pte, uint64_t flags)
{
	*pte = (flags & 0xfff) | ((*pte) & ~0xfff);
}

static inline table_indicies calculate_indicies(uintptr_t virt_addr)
{
	table_indicies ret;
	
	ret.pt = (virt_addr >> 12) & 0x1ff;
	ret.pd = (virt_addr >> 21) & 0x1ff;
	ret.pdp = (virt_addr >> 30) & 0x1ff;
	ret.pml = (virt_addr >> 39) & 0x1ff;
	
	return ret;
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	table_indicies idx = calculate_indicies(virt_addr);
	
	pte *pml = (pte *)runtime_pagetables;
	if (pml[idx.pml] == 0) {
		set_base_address(&pml[idx.pml], (uint64_t)heap_alloc_page());
		set_flags(&pml[idx.pml], 0x7);
	}
	
	pte *pdp = (pte *)get_base_address(&pml[idx.pml]);
	if (pdp[idx.pdp] == 0) {
		set_base_address(&pdp[idx.pdp], (uint64_t)heap_alloc_page());
		set_flags(&pdp[idx.pdp], 0x7);
	}

	pte *pd = (pte *)get_base_address(&pdp[idx.pdp]);
	if (pd[idx.pd] == 0) {
		set_base_address(&pd[idx.pd], (uint64_t)heap_alloc_page());
		set_flags(&pd[idx.pd], 0x7);
	}
	
	pte *pt = (pte *)get_base_address(&pd[idx.pd]);
	set_base_address(&pt[idx.pt], (uint64_t)phys_addr);
	set_flags(&pt[idx.pt], 0x7);
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	arch_abort();
}
