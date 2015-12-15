#include "mem.h"
#include "string.h"

//#define DEBUG_MMU

#ifdef DEBUG_MMU
#define DEBUG(x) uart_puts(x)
#define DEBUGX(x) uart_puthex(x)
#else
#define DEBUG(x) while(0)
#define DEBUGX(x) while(0)
#endif

static char mem_inited = 0;

// This needs to be super aligned
static uint32_t section_table[1 << 12] __attribute__((aligned(16384)));

// MCR FORMAT:
// mcr p15, 0, Rd, CRn, CRm, opc2
// Unless specified, CRm and opc2 SBZ

static void write_ttbr(void* ttbr_val)
{
	// mcr something something
	asm("mcr p15, 0, %0, cr2, cr0, 0" :: "r"(ttbr_val) :);
}

static void write_dacr(uint32_t dacr_val)
{
	asm("mcr p15, 0, %0, cr3, cr0, 0" :: "r"(dacr_val) :);
}

extern void *vectors_start, *vectors_end, *_TEXT_START, *_TEXT_END, *_DATA_START, *_DATA_END;

static int mem_create_region_id_mapping(uintptr_t region_start, uintptr_t region_end)
{
	uintptr_t i;
	size_t page_size = mem_get_page_size();
	
	region_start &= ~(page_size-1);
	
	for(i = region_start; i <= region_end; i += page_size) {
		if(mem_create_page_mapping(i, i)) return 1;
	}
	return 0;
}

static int mem_create_region_device_mapping(uintptr_t region_start, uintptr_t region_end)
{
	uintptr_t i;
	size_t page_size = mem_get_page_size();
	
	region_start &= ~(page_size-1);
	
	for(i = region_start; i <= region_end; i += page_size) {
		if(mem_create_page_mapping_device(i, i)) return 1;
	}
	return 0;
}

void mem_init()
{
	if(mem_inited) return;
	
	// Write L1 page table location to TTbr
	write_ttbr((void*)section_table);
	
	// Zero out L1 page table
	bzero((void*)section_table, sizeof(section_table));
	
	// Initialise page table with mappings for code and data sections
	DEBUG("Mapping vectors\n\r");
	mem_create_region_id_mapping((uintptr_t)&vectors_start, (uintptr_t)&vectors_end);

	DEBUG("Mapping text\n\r");
	mem_create_region_id_mapping((uintptr_t)&_TEXT_START, (uintptr_t)&_TEXT_END);
	
	DEBUG("Mapping data\n\r");
	mem_create_region_id_mapping((uintptr_t)&_DATA_START, (uintptr_t)&_DATA_END);
	
	const phys_mem_info_t *dev_info = mem_get_device_info();
	DEBUG("Mapping devices\n\r");
	while(dev_info) {
		mem_create_region_device_mapping(dev_info->phys_mem_start, dev_info->phys_mem_end);
		dev_info = dev_info->next_mem;
	}
	
	DEBUG("Writing DACR\n\r");
	write_dacr(0xffffffff);
	
	mem_inited = 1;
}

void mem_reset()
{
	mem_inited = 0;
	mem_init();
}

void mem_mmu_enable()
{
	uint32_t c;
	asm("mrc p15, 0, %0, cr1, cr0, 0" : "=r"(c) ::);
	c |= 0x1;
	asm("mcr p15, 0, %0, cr1, cr0, 0" :: "r"(c) :);
}

void mem_mmu_disable()
{
	uint32_t c;
	asm("mrc p15, 0, %0, cr1, cr0, 0" : "=r"(c) ::);
	c &= ~0x1;
	asm("mcr p15, 0, %0, cr1, cr0, 0" :: "r"(c) :);
}

void mem_tlb_flush()
{
	asm("mcr p15, 0, r0, cr8, cr7, 0" :::);
}

void mem_tlb_evict(uintptr_t ptr)
{
	asm("mcr p15, 0, %0, cr8, cr7, 1" ::"r"(ptr):);
}

// Support only 1MB section mappings for now
size_t mem_get_page_size()
{
	return 1 << 20;
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) return 1;
	
	DEBUG("\n\rMapping ");
	DEBUGX(phys_addr);
	DEBUG(" to ");
	DEBUGX(virt_addr);
	DEBUG("\n\r");
	
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, cacheable, buffered (outer and inner write back, no write allocate)
	uint32_t descriptor = (phys_addr & 0xfff00000) | (0x3 << 10) | 0xc | 0x2;
	
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	*table_entry_ptr = descriptor;
	
	return 0;
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) return 1;
	
	DEBUG("\n\rMapping ");
	DEBUGX(phys_addr);
	DEBUG(" to ");
	DEBUGX(virt_addr);
	DEBUG("\n\r");
	
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, non cached, non buffered (strongly ordered)
	uint32_t descriptor = (phys_addr & 0xfff00000) | (0x3 << 10) | 0x2;
	
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	*table_entry_ptr = descriptor;
	
	return 0;
}
