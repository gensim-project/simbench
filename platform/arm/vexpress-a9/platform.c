#include "platform.h"
#include "uart.h"
#include "printf.h"
#include "constants.h"

void platform_init()
{
	uart_init();
	printf_register_output(uart_putc);
	printf_register_debug(uart_putc);
	printf_register_error(uart_putc);
	}

void platform_shutdown()
{
	while(1);
}

uint32_t* platform_get_null_devptr()
{	
	// Return CHIP ID register
	return 0x10000000;
}

static char toggle = 0;
uint32_t platform_get_null_devval()
{
	// Return value to enable all LEDs
	return 0;
}

#define CPU_PERIPHBASE 0x1e000000
#define GICD_BASE (CPU_PERIPHBASE + 0x1000)
#define GICD_CTLR *(volatile uint32_t*)(GICD_BASE + 0x000)
#define GICD_ISENABLER0 *(volatile uint32_t*)(GICD_BASE + 0x100)
#define GICD_IPRIORITYR0 *(volatile uint32_t*)(GICD_BASE + 0x400)
#define GICD_SGIR *(volatile uint32_t*)(GICD_BASE + 0x0f00)
#define GICD_CPENDSGIR0 *(volatile uint32_t*)(GICD_BASE + 0xf10)
#define GICD_PIDR0 *(volatile uint32_t*)(GICD_BASE + 0xfe0)
#define GICD_PIDR1 *(volatile uint32_t*)(GICD_BASE + 0xfe4)
#define GICD_PIDR2 *(volatile uint32_t*)(GICD_BASE + 0xfe8)
#define GICD_PIDR3 *(volatile uint32_t*)(GICD_BASE + 0xfec)

#define GICC_BASE (CPU_PERIPHBASE + 0x100)
#define GICC_CTLR *(volatile uint32_t*)(GICC_BASE + 0x000)
#define GICC_PMR *(volatile uint32_t*)(GICC_BASE + 0x004)
#define GICC_IAR *(volatile uint32_t*)(GICC_BASE + 0x00C)
#define GICC_EOIR *(volatile uint32_t*)(GICC_BASE + 0x010)

void platform_trigger_swi()
{
	// interrogate GIC
	uint32_t id0 = GICD_PIDR0;
	uint32_t id1 = GICD_PIDR1;
	uint32_t id2 = GICD_PIDR2;
	uint32_t id3 = GICD_PIDR3;
	
	if(id0 != 0x90 || id1 != 0x13 || id2 != 0x4 || id3 != 0x0) {
		fprintf(ERROR, "Could not identify GIC distributor interface!\r\n");
		fprintf(ERROR, "Expected 90 13 4 0, got %x %x %x %x\r\n", id0, id1, id2, id3);
		arch_abort();
	} 
	
	// Interrupt should be enabled by reset value of GICD_ISENABLER0
	// but just to be sure
	GICD_ISENABLER0 = 0xffff;
	
	GICD_IPRIORITYR0 = 0xa0a0a0a0;
	
	// direct SGI0 to CPU0 GICD_ITARGETSR0 register
	// (these are read only)
	
	// Set interrupt priority mask register
	GICC_PMR = 0xf0;

	// enable forwarding of interrupts on GICD_CTLR register
	GICD_CTLR |= 0x3;
	
	// enable forwarding of interrupts to CPUs
	GICC_CTLR |= 0x3;
	
	
	// generate SGI0
	uint32_t targetlistfilter = 0x0;
	uint32_t cputargetlist = 1;
	uint32_t nsatt = 0;
	uint32_t sgiintid = 0;
	
	GICD_SGIR = sgiintid | (nsatt << 15) | (cputargetlist << 16) | (targetlistfilter << 24);
}

void platform_clear_swi()
{
	uint32_t intid = GICC_IAR;
	GICD_CPENDSGIR0 = 0xff;
	GICC_EOIR = intid;
}
