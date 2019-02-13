
#ifndef GPIO_H
#define GPIO_H


#ifndef STM8S_H
#error "STM8S.H must be included before GPIO.H"
#endif


/*

//#define _MEM_(R) (*(volatile uint8_t *)R)

#define PA_ODR    0x5000
#define PA_DDR    0x5002
#define PA_CR1    0x5003
#define PA_CR2    0x5004

#define PB_ODR    0x5005
#define PB_DDR    0x5007
#define PB_CR1    0x5008
#define PB_CR2    0x5009

#define PC_ODR    0x500A
#define PC_DDR    0x500C
#define PC_CR1    0x500D
#define PC_CR2    0x500E


#define PD_ODR    0x500f
#define PD_DDR    0x5011
#define PD_CR1    0x5012
#define PD_CR2    0x5013

#define EXTI_CR1	0x50A0
#define EXTI_CR2	0x50A1
*/


#define PORTA PA_BASE_ADDRESS
#define PORTB PB_BASE_ADDRESS
#define PORTC PC_BASE_ADDRESS
#define PORTD PD_BASE_ADDRESS

#define ODR(port) (port+0) 
#define IDR(port) (port+1) 
#define DDR(port) (port+2) 
#define CR1(port) (port+3) 
#define CR2(port) (port+4) 


#define BIT0 1
#define BIT1 2
#define BIT2 4
#define BIT3 8
#define BIT4 16
#define BIT5 32
#define BIT6 64
#define BIT7 127

#define PIN0  0
#define PIN1  1
#define PIN2  2
#define PIN3  3
#define PIN4  4
#define PIN5  5
#define PIN6  6
#define PIN7  7

#define INPUT 0
#define OUTPUT 1
#define HIGH 1
#define LOW 0

#define SetBit(MEM,BIT)     _MEM_(MEM)  |= 1 << BIT ;
#define ClearBit(MEM,BIT)     _MEM_(MEM)  &= ~(1 << BIT);

#define PinInput(PORT,PIN)   ClearBit(DDR(PORT),PIN)
#define PinInputFloat(PORT,PIN)   ClearBit(CR1(PORT),PIN)
#define PinInputPullUp(PORT,PIN)   SetBit(CR1(PORT),PIN)
uint8_t PinRead(uint16_t port,uint8_t pin);

#define PinOutput(PORT,PIN)  SetBit(DDR(PORT),PIN)
#define PinOutputFloat(PORT,PIN)  ClearBit(CR1(PORT),PIN)
#define PinOutputPullUp(PORT,PIN)  SetBit(CR1(PORT),PIN)
#define PinOutputSpeed2MHZ(PORT,PIN) ClearBit(CR2(PORT),PIN)
#define PinOutputSpeed10MHZ(PORT,PIN) SetBit(CR2(PORT),PIN)

#define PinHigh(PORT,PIN) SetBit(ODR(PORT),PIN)
#define PinLow(PORT,PIN) ClearBit(ODR(PORT),PIN)

#endif
