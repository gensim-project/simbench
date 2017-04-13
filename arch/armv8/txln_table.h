#ifndef ARMV8_TXLN_TABLE_H
#define ARMV8_TXLN_TABLE_H

#include "define.h"

#define PA_BITS 48
#define VA_BITS 48

int mem_create_page_mapping(uintptr_t phys_addr, uintptr_t virt_addr);
int mem_create_page_mapping_device(uintptr_t phys_addr, uintptr_t virt_addr);

void *get_txln_table();
void clear_txln_table();

#endif
