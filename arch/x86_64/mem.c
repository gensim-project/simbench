#include "arch.h"
#include "mem.h"
#include "irq.h"
#include "x86.h"
#include "debug.h"
#include "heap.h"

static uintptr_t initial_pagetables;
static uintptr_t runtime_pagetables;

extern char _TEXT_START, _TEXT_END, _DATA_START, _DATA_END, _HEAP_START, _HEAP_SIZE;

#define PAGE_ADDR(a) (uintptr_t)&(a)
#define NR_PAGES(a, b) ((((uintptr_t)&(b) - (uintptr_t)&(a)) / 0x1000) + 1)

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
	// Map the .text section
	map_page_ranges(PAGE_ADDR(_TEXT_START), PAGE_ADDR(_TEXT_START), NR_PAGES(_TEXT_START, _TEXT_END));

	// Map the .data section
	map_page_ranges(PAGE_ADDR(_DATA_START), PAGE_ADDR(_DATA_START), NR_PAGES(_DATA_START, _DATA_END));
	
	// Map the heap
	map_page_ranges(PAGE_ADDR(_HEAP_START), PAGE_ADDR(_HEAP_START), ((uintptr_t)&_HEAP_SIZE) >> 12);
	
	// Map the LAPIC
	mem_create_page_mapping((uintptr_t)LAPIC, (uintptr_t)LAPIC);
}

static int mem_inited;

void mem_init()
{
	if (mem_inited) return;
	mem_inited = 1;
	
	irq_reset_page_fault_handler();
	
	initial_pagetables = (uintptr_t)0xb000;
	runtime_pagetables = (uintptr_t)heap_alloc_page();
	
	prepare_runtime_pagetables();
}

void mem_reset()
{
	irq_reset_page_fault_handler();
	
	// TODO: Reset page mappings?
}

static inline void write_cr3(uintptr_t addr)
{
	asm volatile("mov %0, %%cr3\n" :: "r"(addr));
}

static inline uintptr_t read_cr3()
{
	uintptr_t addr;
	asm volatile("mov %%cr3, %0\n" : "=r"(addr));
	
	return addr;
}

void mem_mmu_enable()
{
	write_cr3(runtime_pagetables);
}

void mem_mmu_disable()
{
	write_cr3(initial_pagetables);
}

void mem_tlb_flush()
{
	write_cr3(read_cr3());
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
	unsigned int pml, pdp, pd, pt;
} table_indicies;

typedef uint64_t pte;

#define PF_PRESENT		1
#define PF_WRITABLE		2
#define PF_USER			4

static inline void set_base_address(pte *pte, uintptr_t base_addr)
{
	*pte = (base_addr & ~0xfffULL) | ((*pte) & 0xfffULL);
}

static inline uintptr_t get_base_address(const pte *pte)
{
	return (*pte) & ~0xfffULL;
}

static inline void set_flags(pte *pte, uint64_t flags)
{
	*pte = (flags & 0xfffULL) | ((*pte) & ~0xfffULL);
}

static inline uint64_t get_flags(const pte *pte)
{
	return (*pte) & 0xfffULL;
}

static inline int is_present(const pte *pte)
{
	return !!(get_flags(pte) & PF_PRESENT);
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

static int __create_page_mapping(uintptr_t pml4, uintptr_t phys_addr, uintptr_t virt_addr)
{
	table_indicies idx = calculate_indicies(virt_addr);
	
	pte *pml = &((pte *)pml4)[idx.pml];
	if (!is_present(pml)) {
		if (get_base_address(pml) == 0) {
			set_base_address(pml, (uintptr_t)heap_alloc_page());
		}
		
		set_flags(pml, PF_PRESENT | PF_WRITABLE | PF_USER);
	}
	
	pte *pdp = &((pte *)get_base_address(pml))[idx.pdp];
	if (!is_present(pdp)) {
		if (get_base_address(pdp) == 0) {
			set_base_address(pdp, (uintptr_t)heap_alloc_page());
		}
		
		set_flags(pdp, PF_PRESENT | PF_WRITABLE | PF_USER);
	}

	pte *pd = &((pte *)get_base_address(pdp))[idx.pd];
	if (!is_present(pd)) {
		if (get_base_address(pd) == 0) {
			set_base_address(pd, (uintptr_t)heap_alloc_page());
		}
		
		set_flags(pd, PF_PRESENT | PF_WRITABLE | PF_USER);
	}
	
	pte *pt = &((pte *)get_base_address(pd))[idx.pt];
	set_base_address(pt, phys_addr);
	set_flags(pt, PF_PRESENT | PF_WRITABLE | PF_USER);
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	__create_page_mapping(runtime_pagetables, phys_addr, virt_addr);
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	arch_abort();
}

void early_mem_init()
{
	__create_page_mapping(0xb000, (uintptr_t)LAPIC, (uintptr_t)LAPIC);
}
