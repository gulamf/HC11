#ifndef INTERRUPT_H
#define INTERRUPT_H

#ifndef GPIO_H
#error "gpio.h must be included before interrupt.h"
#endif


#define PinInputInterruptDisable(PORT,PIN) ClearBit(CR2(PORT),PIN)
#define PinInputInterruptEnable(PORT,PIN)  SetBit(CR2(PORT),PIN)


#define FALLING_AND_LOW 0b00
#define RISING_ONLY	0b01
#define FALLING_ONLY	0b10
#define RISING_AND_FALLING 0b11

// turn off BITS, then set them
#define PortAIntSens(MODE) EXTI_CR1 &= 0b11111100 ; EXTI_CR1 |= MODE
#define PortBIntSens(MODE) EXTI_CR1 &= 0b11110011 ; EXTI_CR1 |= MODE << 2
#define PortCIntSens(MODE) EXTI_CR1 &= 0b11001111 ; EXTI_CR1 |= MODE << 4
#define PortDIntSens(MODE) EXTI_CR1 &= 0b00111111 ; EXTI_CR1 |= MODE << 6


#define TopLeveIntSensFalling() EXTI_CR2 &= 0b11111011
#define TopLeveIntSensRising() EXTI_CR2  |= 0b00000100

#define ISR_TLI(name) void name(void) __interrupt(TLI_ISR)
#define ISR_PORTA(name) void name(void) __interrupt(EXTI0_ISR)
#define ISR_PORTB(name) void name(void) __interrupt(EXTI1_ISR)
#define ISR_PORTC(name) void name(void) __interrupt(EXTI2_ISR)
#define ISR_PORTD(name) void name(void) __interrupt(EXTI3_ISR)

#endif
