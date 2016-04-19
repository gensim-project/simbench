#include "timer.h"
#include "printf.h"

extern void pit_init_oneshot(uint16_t);
extern void pit_start(void);
extern void pit_stop(void);
extern int pit_expired();

extern void lapic_timer_start(void);
extern void lapic_timer_stop(void);
extern void lapic_timer_reset(void);
extern uint32_t lapic_timer_read(void);

#define PIT_FREQUENCY 		1193180
#define CALIBRATION_FREQUENCY	100

uint32_t lapic_frequency;

void timer_init()
{
	uint32_t period = (PIT_FREQUENCY / CALIBRATION_FREQUENCY);
	lapic_timer_reset();

	pit_init_oneshot(period);
	pit_start();

	lapic_timer_start();
	while (!pit_expired());
	lapic_timer_stop();

	pit_stop();

	uint32_t ticks_per_period = (0xffffffff - lapic_timer_read());
	ticks_per_period <<= 4;

	lapic_frequency = ticks_per_period / 100;
}

void timer_start()
{
	lapic_timer_start();
}

void timer_reset()
{
	lapic_timer_reset();
}

uint64_t timer_read()
{
	return lapic_timer_read();
}
