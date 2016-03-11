#include "mem.h"

static phys_mem_info_t phys_info = {
	.phys_mem_start = 0x04000000,
	.phys_mem_end   = 0x08000000,
	.next_mem = 0
};

const phys_mem_info_t *mem_get_phys_info()
{
	return &phys_info;
}

const phys_mem_info_t *mem_get_device_info()
{
	return NULL;
}

