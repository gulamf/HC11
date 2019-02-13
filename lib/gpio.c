
#include <stdint.h>
#include "stm8s.h"
#include "gpio.h"


uint8_t PinRead(uint16_t port, uint8_t pin){
	uint8_t rv;
	rv = _MEM_(port+1) & ( 1 << pin);
	return rv;
}

