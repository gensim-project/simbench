#include "mem.h"
#include "constants.h"

static phys_mem_info_t phys_info = {
	.phys_mem_start = PHYS_MEM_START,
	.phys_mem_end   = PHYS_MEM_END,
	.next_mem = 0
};

static phys_mem_info_t device_ic_info = {
	.phys_mem_start = IC_START,
	.phys_mem_end = IC_END,
	.next_mem = 0
};

static phys_mem_info_t device_ostimer_info = {
	.phys_mem_start = OSTIMER_START,
	.phys_mem_end = OSTIMER_END,
	.next_mem = &device_ic_info
};

static phys_mem_info_t device_rcnr_info = {
	.phys_mem_start = RTC_START,
	.phys_mem_end = RTC_END,
	.next_mem = &device_ostimer_info
};

static phys_mem_info_t device_uart_info = {
	.phys_mem_start = UART3_MEM_START,
	.phys_mem_end   = UART3_MEM_END,
	.next_mem = &device_rcnr_info
};

const phys_mem_info_t *mem_get_phys_info()
{
	return &phys_info;
}

const phys_mem_info_t *mem_get_device_info()
{
	return &device_uart_info;
}
