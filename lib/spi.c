#include <stdint.h>
#include "stm8s.h"
#include "gpio.h"
#include "spi.h"
#include "stm8s.h"
#include "delay.h"

void SPI_init() {

	SPI_CR1 = 0b01111100;

/* SPI_CR1 

7        6      5       4       3       2        1      0
LSBFIRST SPE    ---- BR [2:0]----       MSTR     CPOL   CPHA
rw       rw     rw                      rw       rw     rw
Bit 7 LSBFIRST: Frame format (1)
    0: MSB is transmitted first
    1: LSB is transmitted first

Bit 6 SPE: SPI enable (2)
    0: Peripheral disabled
    1: Peripheral enabled

Bits 5:3 BR[2:0]: Baud rate control
    000: f MASTER /2
    001: f MASTER /4
    010: f MASTER /8
    011: f MASTER /16
    100: f MASTER /32
    101: f MASTER /64
    110: f MASTER /128
    111: f MASTER /256
    Note: These bits should not be changed when the communication is ongoing.

Bit 2 MSTR: Master selection (1)
    0: Slave configuration
    1: Master configuration

Bit1 CPOL: Clock polarity (1)
    0: SCK to 0 when idle
    1: SCK to 1 when idle

Bit 0 CPHA: Clock phase (1)
    0: The first clock transition is the first data capture edge
    1: The second clock transition is the first data capture edge


*/

	SPI_CR2 = 0b0000000;

   /* SPI_CR2 
7 	6 	5 	4 	3 	 2 	1 	0
BDM 	BDOE 	CRCEN 	CRCNEXT Reserved RXOnly SSM 	SSI
rw 	rw 	rw 	rw 	rw 	 rw 	rw 	rw


Bit 7 BDM: Bidirectional data mode enable
	0: 2-line unidirectional data mode selected
	1: 1-line bidirectional data mode selected

Bit 6 BDOE: Input/Output enable in bidirectional mode
This bit selects the direction of transfer in bidirectional mode when BDM is set to 1.
	0: Input enabled (receive-only mode)
	1: Output enabled (transmit-only mode)
	In master mode, the MOSI pin is used and in slave mode, the MISO pin is used.

Bit 5 CRCEN: Hardware CRC calculation enable
	0: CRC calculation disabled
	1: CRC calculation Enabled
	Note: This bit should be written only when SPI is disabled (SPE = ‘0’) for correct operation

Bit 4 CRCNEXT: Transmit CRC next
	0: Next transmit value is from Tx buffer
	1: Next transmit value is from Tx CRC register

Bit 3 Reserved

Bit 2 RXONLY: Receive only
	0: Full duplex (Transmit and receive)
	1: Output disabled (Receive only mode)
	This bit combined with BDM bit selects the direction of transfer in 2 line uni-directional mode
	This bit is also useful in a multi-slave system in which this particular slave is not accessed, the output
from the accessed slave is not corrupted.

Bit 1 SSM: Software slave management
	0: Software slave management disabled
	1: Software slave management enabled
	When the SSM bit is set, the NSS pin input is replaced with the value coming from the SSI bit

Bit 0 SSI: Internal slave select
	This bit has effect only when SSM bit is set. The value of this bit is forced onto the NSS pin and the I/O
value of the NSS pin is ignored.
	0: Slave mode
	1: Master mode
*/
//    SPI_CR1 = (1 << SPI_CR1_MSTR) | (1 << SPI_CR1_SPE) | (1 << SPI_CR1_BR1);
//    SPI_CR2 = (1 << SPI_CR2_SSM) | (1 << SPI_CR2_SSI);// | (1 << SPI_CR2_BDM) | (1 << SPI_CR2_BDOE);
}

uint8_t SPI_write(uint8_t data) {
    SPI_DR = data;
    while (!(SPI_SR & (1 << SPI_SR_TXE)));
    _delay_us(50);
    while (!(SPI_SR & (1 << SPI_SR_RXNE)));
    return SPI_DR;
}
