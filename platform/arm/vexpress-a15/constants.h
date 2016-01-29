#ifndef _ARM_CONSTANTS_
#define _ARM_CONSTANTS_

#define LEDS_ADDR      0x11400260
#define LEDS_VAL       0xFFFFFFFF

#define PHYS_MEM_START 0xa0000000
#define PHYS_MEM_END   0xc0000000

#define TTY_UART_BASE         0x1c090000
#define TTY_UART_MEM_START    0x1c090000
#define TTY_UART_MEM_END      0x1c090fff

#define GIC_MEM_START         0x2c000000
#define GIC_MEM_END           0x2c100000

#endif
