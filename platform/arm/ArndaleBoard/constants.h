#ifndef _ARM_CONSTANTS_
#define _ARM_CONSTANTS_

/* *** ** ** * * * ** *  * ** * * * * * * * * * * 

   ArdaleBoard LED configuration

   GPY6     Base Address: 0x1140_0000

   GPY6CON Address = Base Address + 0x0260

   GPY6CON[4] [19:16] = 0x02 = EBI_DATA[12]


  *** ** ** * * * ** *  * ** * * * * * * * * * * */

#define GPIO_BASE      (0x11400000)
#define GPY6CON_ADDR   (GPIO_BASE + 0x0260)
#define GPY6DAT_ADDR   (GPIO_BASE + 0x0264)
#define GPY6PUD_ADDR   (GPIO_BASE + 0x0268)
#define GPY6DRV_ADDR   (GPIO_BASE + 0x026C)

#define LEDS_ADDR      0x11400260
#define LEDS_VAL       0xFFFFFFFF

#define PHYS_MEM_START 0x50000000
#define PHYS_MEM_END   0x5FFFFFFF

#define TTY_UART_BASE         0x12C20000
#define TTY_UART_MEM_START    0x12C20000
#define TTY_UART_MEM_END      0x12C2FFFF

#define GIC_MEM_START         0x10481000
#define GIC_MEM_END           0x10489000

#endif
