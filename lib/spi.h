#ifndef SPI_H
#define SPI_H

#ifndef GPIO_H
#error "gpio.h must be included before spi.h"
#endif


/*
 * Initialize SPI in MODE1.
 *
 * Pinout:
 * SCK  -> PC5
 * MOSI -> PC6
 * MISO -> PC7
 * CS   -> user defined
 */

#define PORT_MOSI PORTC
#define PORT_MISO PORTC
#define PORT_SCK  PORTC
#define PIN_MOSI  PIN6
#define PIN_MISO  PIN7
#define PIN_SCK   PIN5

void SPI_init();

uint8_t SPI_write(uint8_t data);

extern uint16_t spi_read_ok;

#endif /* SPI_H */

