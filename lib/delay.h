#ifndef DELAY_H
#define DELAY_H

#ifndef F_CPU
#warning "F_CPU not defined, using 2MHz by default"
#define F_CPU 2000000UL
#endif

#include <stdint.h>
// DONT RELY ON FOR ACCURATE TIMINGS
inline void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ((F_CPU / 12 / 1000UL) * ms); i++) {
        __asm__("nop");
    }
}


// DONT RELY ON FOR ACCURATE TIMINGS
inline void _delay_cycl( unsigned short __ticks )
{
#if defined(__CSMC__)
/* COSMIC */
  #define T_COUNT(x) (( F_CPU * x / 1000000UL )-3)/3)
	// ldw X, __ticks ; insert automaticaly
	_asm("nop\n $N:\n decw X\n jrne $L\n nop\n ", __ticks);
#elif defined(__SDCC)
  #define T_COUNT(x) (( F_CPU * x / 1000000UL )-5)/5)
	__asm__("nop\n nop\n"); 
	do { 		// ASM: ldw X, #tick; lab$: decw X; tnzw X; jrne lab$
                __ticks--;//      2c;                 1c;     2c    ; 1/2c   
        } while ( __ticks );
	__asm__("nop\n");
#elif defined(__RCST7__)
/* RAISONANCE */
  #error ToDo for RAISONANCE
#elif defined(__ICCSTM8__)
/* IAR */
  #error ToDo for IAR
#else
 #error Unsupported Compiler!          /* Compiler defines not found */
#endif
}

// DONT RELY ON FOR ACCURATE TIMINGS
inline void _delay_us(  unsigned short __us )
{
	_delay_cycl( (unsigned short)( T_COUNT(__us) );
}


// THIS IS ACTUALY TWICE AS LONG!!!
//DONT RELY ON FOR ACCURATE TIMINGS
inline void _delay_ms( unsigned short __ms )
{
	while ( __ms-- )
	{
		_delay_us( 1000 );
	}
}



#endif /* DELAY_H */
