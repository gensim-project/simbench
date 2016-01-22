#include "mem.h"
#include "string.h"
#include "printf.h"
#include "debug.h"

#define DEBUG_MEM

#if !defined(SIMBENCH_DEBUG) && defined(DEBUG_MEM)
#warning "DEBUG_MEM defined, but SIMBENCH_DEBUG not defined"
#endif

#ifdef DEBUG_MEM
#define mem_dprintf(a...) dprintf(a)
#else
#define mem_dprintf(a...) do { } while(0)
#endif

static char mem_inited = 0;

static char mem_tlb_unified = 0;
static char mem_cache_unified = 0;
static char mem_cache_implemented = 0;

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
			mem_dprintf("Failed to map a device at %p!\r\n", i);
			return 1;
		}
	}
	return 0;
}

static uint8_t mem_get_arm_arch_version()
{
	// details here from ARMv5 architecture reference manual (DDI 0100I)
	
	uint32_t ctrl;
	// get CPUID
	asm("mrc p15, 0, %0, cr0, cr0, 0" : "=r"(ctrl));
	
	// if bit 19 is 0, we have <= ARMv4
	if(!(ctrl & (1 << 19))) {
		// if bit 23 is 0, we have ARMv3
		if(!(ctrl & (1 << 23))) return 3;
		
		// otherwise, ARMv4
		return 4;
	}
	
	//extract the architecture field
	ctrl >>= 16;
	ctrl &= 0xf;
	
	switch(ctrl) {
		case 1: 
		case 2:
			return 4;
		case 3:
		case 4:
		case 5:
		case 6:
			return 5;
		case 7:
			return 6;
		case 0xF:
			// Not strictly true
			return 7;
	}
	
	// Unknown version
	return 0;
}

static void mem_check_is_tlb_unified()
{
	uint32_t ctrl;
	// TLB type register only exists since ARMv5
	if(mem_get_arm_arch_version() < 5) {
		mem_tlb_unified = 0;
	} else {
		asm("mrc p15, 0, %0, cr0, cr0, 3" : "=r"(ctrl) ::);
		if(ctrl & 1) mem_tlb_unified = 0;
		else mem_tlb_unified = 1;
	}
}

static void mem_check_is_cache_unified()
{
	uint32_t ctrl;
	
	// cache type register only exists since ARMv5
	switch(mem_get_arm_arch_version()) {
		case 0 ... 4:
			//fprintf(DEBUG, "Cache not unified\r\n");
			mem_cache_unified = 0;
			break;
		case 5 ... 6: {
			asm("mrc p15, 0, %0, cr0, cr0, 1" : "=r"(ctrl) ::);
			//fprintf(DEBUG, "Cache type register %08x\n", ctrl);
			
			uint32_t cache_size = ctrl & 0xfff;
			if(((cache_size >> 2) & 0xf) == 0x1) mem_cache_implemented = 0;
			else mem_cache_implemented = 1;
			
			ctrl = (ctrl >> 24) & 1;
			if(ctrl) mem_cache_unified = 0;
			else mem_cache_unified = 1;
			break;
		}
		case 7:
			// assume a cache - it'll be handled by the cache cleaning ops
			//fprintf(DEBUG, "We have an ARMv7-style cache\r\n");
			mem_cache_implemented = 1;
			break;
	}
}

void mem_init()
{	
	mem_check_is_tlb_unified();
	mem_check_is_cache_unified();
	
	// Write L1 page table location to TTbr
	write_ttbr((void*)section_table);
	
	// Zero out L1 page table
	bzero((void*)section_table, sizeof(section_table));
	
	// Initialise page table with mappings for code and data sections
	mem_dprintf("Mapping vectors\r\n");
	mem_create_region_id_mapping((uintptr_t)&vectors_start, (uintptr_t)&vectors_end);

	mem_dprintf("Mapping text\r\n");
	mem_create_region_id_mapping((uintptr_t)&_TEXT_START, (uintptr_t)&_TEXT_END);
	
	mem_dprintf("Mapping data\r\n");
	mem_create_region_id_mapping((uintptr_t)&_DATA_START, (uintptr_t)&_DATA_END);
	
	const phys_mem_info_t *dev_info = mem_get_device_info();
	mem_dprintf("Mapping devices\r\n");
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
	mem_dprintf("Got control word %x\r\n", ctrl);

	if(ctrl & (1 << 13)) {
		// Vectors are high
		vector_vaddr = 0xffff0000;
		vector_vpage = vector_vaddr & ~(mem_get_page_size()-1);
		mem_dprintf("High vectors detected @ %p\r\n", vector_vaddr);
		
	} else {
		// Vectors are low
		vector_vaddr = 0;
		vector_vpage = 0;
		mem_dprintf("Low vectors detected\r\n");
	}
	
	// Make sure that low bits of virtual and physical vector bases match
	if((vector_vaddr & (mem_get_page_size()-1)) != (vector_paddr & (mem_get_page_size()-1))) {
		fprintf(ERROR, "Unable to map vectors: virtual and physical page offsets do not match!\r\n");
		fprintf(ERROR, "%x\r\n", (vector_vaddr & (mem_get_page_size()-1)));
		fprintf(ERROR, "%x\r\n", (vector_paddr & (mem_get_page_size()-1)));
		arch_abort();
	}
	
	mem_dprintf("Mapping vectors\r\n");	
	mem_create_page_mapping(vector_ppage, vector_vpage);
	
	// Also ID map the physical location of the vectors
	uint32_t vectorstart = (uintptr_t)&_VECTORS_RELOCATE & ~(mem_get_page_size()-1);
	mem_create_region_id_mapping(vectorstart, vectorstart + mem_get_page_size());
	
	// (Note that the vectors now have two virtual mappings: one with VA=PA, and one with VA=Exception Vector Base)
	
	mem_dprintf("Writing DACR\r\n");
	write_dacr(0xffffffff);
	mem_dprintf("Wrote DACR\r\n");
	
	mem_inited = 1;
	
	mem_dprintf("Done initialising memory\r\n");
}

void mem_reset()
{
	mem_inited = 0;
	mem_init();
}

void mem_mmu_enable()
{
	uint32_t c;
	mem_cache_flush();
	
	asm volatile ("mrc p15, 0, %0, cr1, cr0, 0" : "=r"(c) ::);
	c |= 0x1;
	// also enable D$, WB, L2
	c |= (1 << 2) | (1 << 3) | (1 << 26) | (1 << 14);
	asm volatile (".align 5 \n"
	              "mcr p15, 0, %0, cr1, cr0, 0\n"
	              "mrc p15, 0, %1, cr1, cr0, 0" : "=r"(c) : "r"(c) :);
	asm volatile ("mcr p15, 0, %0, cr7, cr5, 4" :: "r"(0) :); //PrefetchFlush
	asm volatile ("mcr p15, 0, %0, cr7, cr5, 6" :: "r"(0) :); //BTB Flush
	mem_cache_flush();
}

extern void mem_arm_cache_mmu_off();
void mem_mmu_disable()
{
	//fprintf(DEBUG, "Disabling MMU\r\n");
	mem_arm_cache_mmu_off();
}

void mem_tlb_flush()
{
	asm volatile ("mcr p15, 0, %0, cr8, cr7, 0" :: "r"(0):);
	asm volatile ("mcr p15, 0, %0, cr7, cr5, 4" :: "r"(0) :); //PrefetchFlush
}

void mem_tlb_evict(uintptr_t ptr)
{
	if(mem_tlb_unified) {
		asm("mcr p15, 0, %0, cr8, cr7, 1" ::"r"(ptr):);
	} else {
		asm("mcr p15, 0, %0, cr8, cr5, 1" ::"r"(ptr):);
		asm("mcr p15, 0, %0, cr8, cr6, 1" ::"r"(ptr):);
	}
}

extern void mem_arm_flush_dcache();
void mem_cache_flush()
{
	// fprintf(DEBUG, "Flushing D$\r\n");
	if(!mem_cache_implemented) {
		//fprintf(DEBUG, "No cache implemented\r\n");
		return;
	}

	// Older versions of the ARM architecture support cleaning the 
	// entire D cache in a single operation
	if(mem_get_arm_arch_version() <= 5) {
		// Clean entire data and unified cache
		//fprintf(DEBUG, "Flushing cache (<=v5)\r\n");
		if(mem_cache_unified) {
			asm("mcr p15, 0, %0, cr7, cr15, 0" :: "r"(0):);
		} else {
			asm("mcr p15, 0, %0, cr7, cr14, 0" :: "r"(0):);
		}
	} else {
		//fprintf(DEBUG, "Flushing cache (v7)\r\n");
		mem_arm_flush_dcache();
	}
	
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
	mem_dprintf("Mapping va=%p to pa=%p\r\n", virt_addr, phys_addr);

	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) {
		dprintf(" *** TRIED TO MAP A MISALIGNED ADDRESS! *** \r\n");
		return 1;
	}
	
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, cacheable, buffered (outer and inner write back, no write allocate)
	uint32_t base = (phys_addr & 0xfff00000);
	uint32_t C = 1;
	uint32_t B = 1;
	uint32_t domain = 0;
	uint32_t AP = 0x3;
	uint32_t TEX = 0;
	uint32_t descriptor = base | (TEX << 12) | (AP << 10) | (domain << 5) | (C << 3) | (B << 2) | 0x2;
		
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	
	// Check to make sure we aren't overwriting an existing mapping
	if(*table_entry_ptr && *table_entry_ptr != descriptor) {
		fprintf(ERROR, "ERROR! Overwriting mapping for address %p! (0x%08x)\n", virt_addr, *table_entry_ptr);
		arch_abort();
	}
	
	*table_entry_ptr = descriptor;
	
	return 0;
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	mem_dprintf("Mapping Device va=%p to pa=%p\r\n", virt_addr, phys_addr);
	
	// Ensure that phys and virt addrs are page aligned
	if(phys_addr & (mem_get_page_size()-1) || virt_addr & (mem_get_page_size()-1)) {
		mem_dprintf(" *** TRIED TO DEVICE MAP A MISALIGNED ADDRESS! *** \r\n");
		return 1;
	}
	// Create a section descriptor pointing to the given physical address
	// with full access permissions, domain 0, non cached, buffered (device)
	uint32_t base = (phys_addr & 0xfff00000);
	uint32_t C = 0;
	uint32_t B = 0;
	uint32_t domain = 0;
	uint32_t AP = 0x3;
	uint32_t TEX = 0;
	uint32_t descriptor = base | (TEX << 12) | (AP << 10) | (domain << 5) | (C << 3) | (B << 2) | 0x2;
	
	uint32_t *table_entry_ptr = &section_table[virt_addr >> 20];
	if(*table_entry_ptr && *table_entry_ptr != descriptor) {
		fprintf(ERROR, "ERROR! Overwriting mapping for address %p! (0x%08x)\n", virt_addr, *table_entry_ptr);
		arch_abort();
	}
	
	*table_entry_ptr = descriptor;
	
	return 0;
}
