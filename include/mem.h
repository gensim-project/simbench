#ifndef MEM_H
#define MEM_H

void mem_init();
void mem_reset();

void mem_mmu_enable();
void mem_mmu_disable();

void mem_tlb_flush();
void mem_tlb_evict(void *ptr);

#endif
