#include "stm8s.h"
#include "timer.h"

#define INTERVAL  

void tim1_init(void) {
     // 1/32 second interval
     TIM1_PSCRH = 0x00; // HIGH BYTE of 1/32 second interval
     TIM1_PSCRL = 0x20; // LOW BYTE of 1/32 second interval

     TIM1_CR1 = 0x01; //Enable timer
     TIM1_IER = 0x01; //Enable interrupt - update event

}

