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

extern uint32_t _VECTORS_RELOCATE;

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
		if(mem_create_page_mapping_device(i, i)) {
			DEBUG("Failed to map a device at ");
			DEBUGX(i);
			DEBUG("!\r\n");
			
			return 1;
		}
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
	DEBUG("Mapping vectors\r\n");
	mem_create_region_id_mapping((uintptr_t)&vectors_start, (uintptr_t)&vectors_end);

	DEBUG("Mapping text\r\n");
	mem_create_region_id_mapping((uintptr_t)&_TEXT_START, (uintptr_t)&_TEXT_END);
	
	DEBUG("Mapping data\r\n");
	mem_create_region_id_mapping((uintptr_t)&_DATA_START, (uintptr_t)&_DATA_END);
	
	const phys_mem_info_t *dev_info = mem_get_device_info();
	DEBUG("Mapping devices\r\n");
	while(dev_info) {
		mem_create_region_device_mapping(dev_info->phys_mem_start, dev_info->phys_mem_end);
		dev_info = dev_info->next_mem;
	}
	
	// Attempt to map exception vectors
	// First, determine whether vectors are high or low
	uint32_t ctrl;
	uintptr_t vector_vaddr;
	uintptr_t vector_vpage;
	uintptr_t vector_paddr = (uintptr_t)&_VECTORS_RELOCATE;
	uintptr_t vector_ppage = vector_paddr & ~(mem_get_page_size()-1);
	asm("mrc p15, 0, %0, cr1, cr0, 0\n" : "=r"(ctrl));
	DEBUG("Got control word ");
	DEBUGX(ctrl);
	DEBUG("\r\n");
	if(ctrl & (1 << 13)) {
		// Vectors are high
		vector_vaddr = 0xffff0000;
		vector_vpage = vector_vaddr & ~(mem_get_page_size()-1);
		DEBUG("High vectors detected\r\n");
		DEBUGX(vector_vaddr);
		
	} else {
		// Vectors are low
		vector_vaddr = 0;
		vector_vpage = 0;
		DEBUG("Low vectors detected\r\n");
	}
	
	// Make sure that low bits of virtual and physical vector bases match
	if((vector_vaddr & (mem_get_page_size()-1)) != (vector_paddr & (mem_get_page_size()-1))) {
		uart_puts("Unable to map vectors: virtual and physical page offsets do not match!\r\n");
		uart_puthex((vector_vaddr & (mem_get_page_size()-1)));
		uart_puts("\r\n");
		uart_puthex((vector_paddr & (mem_get_page_size()-1)));
		uart_puts("\r\n");
		while(1) ;
	}
	
	DEBUG("Mapping vectors\r\n");	
	mem_create_page_mapping(vector_ppage, vector_vpage);
	
	// Also ID map the physical location of the vectors
	uint32_t vectorstart = (uintptr_t)&_VECTORS_RELOCATE & ~(mem_get_page_size()-1);
	mem_create_region_id_mapping(vectorstart, vectorstart + mem_get_page_size());
	
	// (Note that the vectors now have two virtual mappings: one with VA=PA, and one with VA=Exception Vector Base)
	
	DEBUG("Writing DACR\r\n");
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
	
	mem_cache_flush();
}

void mem_mmu_disable()
{
	uint32_t c;
	asm("mrc p15, 0, %0, cr1, cr0, 0" : "=r"(c) ::);
	c &= ~0x1;
	asm("mcr p15, 0, %0, cr1, cr0, 0" :: "r"(c) :);
	
	mem_cache_flush();
}

void mem_tlb_flush()
{
	asm("mcr p15, 0, r0, cr8, cr7, 0" :::);
}

void mem_tlb_evict(uintptr_t ptr)
{
	asm("mcr p15, 0, %0, cr8, cr7, 1" ::"r"(ptr):);
}

void mem_cache_flush()
{
	// Clean entire data and unified cache
	asm("mcr p15, 0, %0, cr7, cr14, 0" :: "r"(0):);
	asm("mcr p15, 0, %0, cr7, cr15, 0" :: "r"(0):);
	
	// Drain write buffer (DSB)
	asm("mcr p15, 0, %0, cr7, cr10, 4" :: "r"(0):);
	
	// Prefetch Flush
	asm("mcr p15, 0, %0, cr7, cr5, 4" :: "r"(0):);
}

// Support only 1MB section mappings for now
size_t mem_get_page_size()
{
	return 1 << 20;
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	DEBUG("\r\nMapping ");
	DEBUGX(phys_addr);
	DEBUG(" to ");
	DEBUGX(virt_addr);
	DEBUG("\r\n");

	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) {
		DEBUG(" *** TRIED TO MAP A MISALIGNED ADDRESS! *** \r\n");
		return 1;
	}
	
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, cacheable, buffered (outer and inner write back, no write allocate)
	uint32_t descriptor = (phys_addr & 0xfff00000) | (0x3 << 10) | 0xc | 0x2;
	
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	*table_entry_ptr = descriptor;
	
	return 0;
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	DEBUG("\r\nMapping Device ");
	DEBUGX(phys_addr);
	DEBUG(" to ");
	DEBUGX(virt_addr);
	DEBUG("\r\n");
	
	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) {
		DEBUG(" *** TRIED TO DEVICE MAP A MISALIGNED ADDRESS! *** \r\n");
		return 1;
	}
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, non cached, buffered (device)
	uint32_t descriptor = (phys_addr & 0xfff00000) | (0x3 << 10) | 0x4 | 0x2;
	
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	*table_entry_ptr = descriptor;
	
	return 0;
}
