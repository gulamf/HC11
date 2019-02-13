#ifndef TIMER_H
#define TIMER_H

#ifndef STM8S_H
#error "stm8s.h must be included before timer.h"
#endif


#define ISR_TIM1(name) void name(void) __interrupt(TIM1_OVF_ISR)
#define TIM1_RESART()      TIM1_SR1 &= ~1; //reset interrupt


void tim1_init(void);

#endif
