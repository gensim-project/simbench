#include "txln_table.h"
#include "heap.h"
#include "printf.h"

#define txln_dprintf(...) fprintf(OUTPUT, "txln: " __VA_ARGS__)

#define TXLT_T0SZ (64 - (VA_BITS))

#define TXLT_L0_X (28-(TXLT_T0SZ))
#define TXLT_L0_Y ((TXLT_L0_X) + 35)
#define TXLT_L0_ENTRIES (1 << ((TXLT_L0_Y) - 38))

#define TXLT_L1_X (12)
#define TXLT_L1_Y ((TXLT_L1_X) + 26)
#define TXLT_L1_ENTRIES (1 << ((TXLT_L1_Y) - 29))

#define TXLT_L2_X (12)
#define TXLT_L2_Y ((TXLT_L2_X) + 17)
#define TXLT_L2_ENTRIES (1 << ((TXLT_L2_Y) - 20))

#define TXLT_L3_Y 20
#define TXLT_L3_ENTRIES (1 << ((TXLT_L3_Y) - 11))

typedef struct {
	uint64_t data;
} vmsav8_64_l0_descriptor_t;

typedef struct {
	uint64_t data;
} vmsav8_64_l1_descriptor_t;

typedef struct {
	uint64_t data;
} vmsav8_64_l2_descriptor_t;

typedef struct {
	uint64_t data;
} vmsav8_64_l3_descriptor_t;

typedef struct {
	vmsav8_64_l3_descriptor_t entries[TXLT_L3_ENTRIES];
} vmsav8_64_l3_txln_table_t;

typedef struct {
	vmsav8_64_l2_descriptor_t entries[TXLT_L2_ENTRIES];
} vmsav8_64_l2_txln_table_t;

typedef struct {
	vmsav8_64_l1_descriptor_t entries[TXLT_L1_ENTRIES];
} vmsav8_64_l1_txln_table_t;

typedef struct {
	vmsav8_64_l0_descriptor_t entries[TXLT_L0_ENTRIES];
} vmsav8_64_l0_txln_table_t;

static vmsav8_64_l0_txln_table_t *l0_table;

void *get_txln_table() {
	if(l0_table == 0) l0_table = armv8_heap_get_page();
	return l0_table;
}

static vmsav8_64_l0_txln_table_t *get_l0_table() {
	if(l0_table == 0) l0_table = armv8_heap_get_page();
	return l0_table;
}

static void clear_l0_table(vmsav8_64_l0_txln_table_t *table) {
	for(int i = 0; i < TXLT_L0_ENTRIES; i++) {
		table->entries[i].data &= ~0x3ULL;
	}
}

void clear_txln_table() {
	clear_l0_table(get_l0_table());
}

static void clear_l1_table(vmsav8_64_l1_txln_table_t *table) {
	for(int i = 0; i < TXLT_L1_ENTRIES; i++) {
		table->entries[i].data &= ~0x3ULL;
	}
}

static void clear_l2_table(vmsav8_64_l2_txln_table_t *table) {
	for(int i = 0; i < TXLT_L2_ENTRIES; i++) {
		table->entries[i].data &= ~0x3ULL;
	}
}

static void clear_l3_table(vmsav8_64_l3_txln_table_t *table) {
	for(int i = 0; i < TXLT_L3_ENTRIES; i++) {
		table->entries[i].data &= ~0x3ULL;
	}
}

static vmsav8_64_l0_descriptor_t get_l0_descriptor(vmsav8_64_l0_txln_table_t *table, uintptr_t virt_addr) {
	uint32_t idx = virt_addr >> 39;
	idx &= TXLT_L0_ENTRIES-1;
	vmsav8_64_l0_descriptor_t descriptor = table->entries[idx];
	if((descriptor.data & 0x3) == 0) {
		void *table_page = (void*)(descriptor.data & 0xfffffffff000);
		if(!table_page) {
			table_page = armv8_heap_get_page();
		}
		
		clear_l1_table(table_page);
		
		descriptor.data = (uintptr_t)table_page;
		descriptor.data |= 3;
		table->entries[idx] = descriptor;
	}
	return descriptor;
}

static vmsav8_64_l1_txln_table_t *get_l1_table(vmsav8_64_l0_txln_table_t *l0_table, uintptr_t virt_addr) {
	vmsav8_64_l0_descriptor_t descriptor = get_l0_descriptor(l0_table, virt_addr);
	
	uint64_t table_address = descriptor.data & (0xfffffffff000);
	return (vmsav8_64_l1_txln_table_t*)table_address;
}

static vmsav8_64_l1_descriptor_t get_l1_descriptor(vmsav8_64_l1_txln_table_t *table, uintptr_t virt_addr) {
	uint32_t idx = virt_addr >> 30;
	idx &= TXLT_L1_ENTRIES-1;
	vmsav8_64_l1_descriptor_t descriptor = table->entries[idx];
	if(descriptor.data == 0) {
		void *table_page = (void*)(descriptor.data & 0xfffffffff000);
		if(!table_page) {
			table_page = armv8_heap_get_page();
		}
		
		clear_l2_table(table_page);
		
		descriptor.data = (uintptr_t)table_page;
		descriptor.data |= 3;
		table->entries[idx] = descriptor;
	}
	return descriptor;
}

static vmsav8_64_l2_txln_table_t *get_l2_table(vmsav8_64_l1_txln_table_t *l0_table, uintptr_t virt_addr) {
	vmsav8_64_l1_descriptor_t descriptor = get_l1_descriptor(l0_table, virt_addr);
	
	uint64_t table_address = descriptor.data & (0xfffffffff000);
	return (vmsav8_64_l2_txln_table_t*)table_address;
}

static vmsav8_64_l2_descriptor_t get_l2_descriptor(vmsav8_64_l2_txln_table_t *table, uintptr_t virt_addr) {
	uint32_t idx = virt_addr >> 21;
	idx &= TXLT_L2_ENTRIES-1;
	vmsav8_64_l2_descriptor_t descriptor = table->entries[idx];
	if(descriptor.data == 0) {
		void *table_page = (void*)(descriptor.data & 0xfffffffff000);
		if(!table_page) {
			table_page = armv8_heap_get_page();
		}
		
		clear_l3_table(table_page);
		
		descriptor.data = (uintptr_t)table_page;
		descriptor.data |= 3;
		table->entries[idx] = descriptor;
	}
	return descriptor;
}

static vmsav8_64_l3_txln_table_t *get_l3_table(vmsav8_64_l2_txln_table_t *l0_table, uintptr_t virt_addr) {
	vmsav8_64_l2_descriptor_t descriptor = get_l2_descriptor(l0_table, virt_addr);
	
	uint64_t table_address = descriptor.data & (0xfffffffff000);
	return (vmsav8_64_l3_txln_table_t*)table_address;
}

static vmsav8_64_l3_descriptor_t *get_l3_descriptor(vmsav8_64_l3_txln_table_t *table, uintptr_t virt_addr) {
	uint32_t idx = virt_addr >> 12;
	idx &= TXLT_L3_ENTRIES-1;
	return &table->entries[idx];
}

void insert_mapping(vmsav8_64_l3_txln_table_t *l3_table, uintptr_t virt_addr, uintptr_t phys_addr) {
	vmsav8_64_l3_descriptor_t *descriptor = get_l3_descriptor(l3_table, virt_addr);
	descriptor->data = (phys_addr & ~0xfff) | 0x3;
	// Set read/write from EL0 by setting AP[2:1] = 0b01
	descriptor->data |= (0 << 6);
	// use MAIR index 0
	
	//set access flag
	descriptor->data |= (1 << 10);
}

void insert_mapping_device(vmsav8_64_l3_txln_table_t *l3_table, uintptr_t virt_addr, uintptr_t phys_addr) {
	vmsav8_64_l3_descriptor_t *descriptor = get_l3_descriptor(l3_table, virt_addr);
	descriptor->data = (phys_addr & ~0xfff) | 0x3;	
	// use MAIR index 1
	descriptor->data |= (1 << 2);
	//set access flag
	descriptor->data |= (1 << 10);
}

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr)
{
	txln_dprintf("Creating a mapping from %p to %p\n", virt_addr, phys_addr);
	
	vmsav8_64_l0_txln_table_t *l0_table = get_l0_table();
	vmsav8_64_l1_txln_table_t *l1_table = get_l1_table(l0_table, virt_addr);
	vmsav8_64_l2_txln_table_t *l2_table = get_l2_table(l1_table, virt_addr);
	vmsav8_64_l3_txln_table_t *l3_table = get_l3_table(l2_table, virt_addr);
	
	insert_mapping(l3_table, virt_addr, phys_addr);
	
	return 0;
}

int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr)
{
	txln_dprintf("Creating a device mapping from %p to %p\n", virt_addr, phys_addr);
	
	vmsav8_64_l0_txln_table_t *l0_table = get_l0_table();
	vmsav8_64_l1_txln_table_t *l1_table = get_l1_table(l0_table, virt_addr);
	vmsav8_64_l2_txln_table_t *l2_table = get_l2_table(l1_table, virt_addr);
	vmsav8_64_l3_txln_table_t *l3_table = get_l3_table(l2_table, virt_addr);
	
	insert_mapping_device(l3_table, virt_addr, phys_addr);
	
	return 0;
}
