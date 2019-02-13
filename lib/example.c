#include <stdio.h>
#include "lib/stm8s.h"
#include "lib/delay.h"
#include "lib/gpio.h"
#include "lib/uart.h"
#include "lib/interrupt.h"
#include "lib/spi.h"
#include "lib/timer.h"
#include "lib/eeprom.h"
#include "lib/cc1101.h"

#define SETIO_PORT PORTC
#define SETIO_PIN PIN3

#define RX_PORT PORTD
#define RX_PIN PIN6

#define TX_PORT PORTD
#define TX_PIN	PIN5

#define FALSE 0
#define TRUE 1

volatile uint8_t packetWaiting;

ISR_PORTD(handlerD) {
	c=1;
}


// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
    uint8_t rssi_dec;
    // TODO: This rssi_offset is dependent on baud and MHz; this is for 38.4kbps and 433 MHz.
    uint8_t rssi_offset = 74;
    rssi_dec = (uint8_t) raw;
    if (rssi_dec >= 128)
        return ((int)( rssi_dec - 256) / 2) - rssi_offset;
    else
        return (rssi_dec / 2) - rssi_offset;
}

// Get link quality indicator.
int lqi(char raw) {
    return 0x3F - raw;
}


void main() {
	uint8_t length, uint8_t rssi, uint8_t crc_ok, uint8_t lqi;
	char[64] packet;
// setup
    cc1101_init();
    cc1101_setSyncWord(syncWord);
    cc1101_setCarrierFreq(CFREQ_433);
    cc1101_disableAddressCheck();
    cc1101_setTxPowerAmp(PA_LowPower);

    uart_init();
    printf("CC1101_PARTNUM ");
    printf("%d\n",cc1101_readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
    printf("CC1101_VERSION ");
    printf("%d\n",cc1101_readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
    printf("CC1101_MARCSTATE ");
    printf("%d\n",cc1101_readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

    printf("CC1101 radio initialized."));
    PinInputInterruptEnable(GD0_PORT,GD0_PIN);


// loop
    while(1) {

    if (packetWaiting) {
	PinInputInterruptDisable(GD0_PORT,GD0_PIN);
        packetWaiting = false;
        if (cc1101_receiveData(&packet, &length, &rssi, &crc_ok, &lqi) > 0) {
            printf("Received packet...\n");
            if (!crc_ok) {
                printf("crc not ok\n");
            }
            printf("lqi: ");
            printf("%d ",lqi(packet.lqi));
            printf("rssi: ");
            printf("%d ",rssi(packet.rssi));
            printf("dBm\n");

            if (packet.crc_ok && length > 0) {
                printf("packet: len ");
                printf("%d\n",length);
                printf("data: \n");
                printf("%s\n",(const char *) packet.data);
            }
        }

	PinInputInterruptEnable(GD0_PORT,GD0_PIN);
    }
    unsigned long now = millis();
    if (now > lastSend + sendDelay) {
	PinInputInterruptDisable(GD0_PORT,GD0_PIN);

        lastSend = now;
        const char *message = "hello world2";
        // We also need to include the 0 byte at the end of the string
        length = strlen(message)  + 1;
        strncpy((char *) packet, message, length);

        cc1101_sendData(packet);
        printf("Sent packet...");

	PinInputInterruptEnable(GD0_PORT,GD0_PIN);
    }



    }

}
