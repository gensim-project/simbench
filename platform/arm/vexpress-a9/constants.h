#ifndef _ARM_CONSTANTS_
#define _ARM_CONSTANTS_

#define LEDS_ADDR      0x11400260
#define LEDS_VAL       0xFFFFFFFF

#define PHYS_MEM_START 0x84000000
#define PHYS_MEM_END   0xa0000000

#define TTY_UART_BASE         0x10009000
#define TTY_UART_MEM_START    0x10009000
#define TTY_UART_MEM_END      0x10009fff

#define GIC_MEM_START         0x1e000000
#define GIC_MEM_END           0x1e100000

#endif
