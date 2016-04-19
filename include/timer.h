#ifndef TIMER_H
#define TIMER_H

#include "define.h"

extern void timer_init(void);
extern void timer_reset(void);
extern void timer_start(void);

extern uint64_t timer_read(void);

#endif
