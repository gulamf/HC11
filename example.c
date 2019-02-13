#include <stdio.h>
#include <string.h>
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

uint8_t syncWord[2] = {199, 10};
volatile uint8_t packetWaiting;

unsigned long lastSend = 0;
unsigned int sendDelay = 50000;


ISR_PORTD(handlerD) {
	packetWaiting=TRUE;
}


// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int toRssi(char raw) {
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
int toLqi(char raw) {
    return 0x3F - raw;
}


void main() {
    uint8_t length,  rssi,  crc_ok, lqi;
    char packet[BUFFER_LEN];
    unsigned long now = 0;

// setup

    cc1101_init(CFREQ_433,0);
    cc1101_setSyncWord(syncWord[0],syncWord[1]);
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

    printf("CC1101 radio initialized.");
    
    // set up interrupt
    PinInputInterruptEnable(GD0_PORT,GD0_PIN);
    // Enable PORTD interrupts
    PortDIntSens(FALLING_ONLY);
    enable_interrupts();


// loop
    while(1) {    
        if (packetWaiting) {
    	PinInputInterruptDisable(GD0_PORT,GD0_PIN);
            packetWaiting = FALSE;
            if (cc1101_receiveData(packet, &length, &rssi, &crc_ok, &lqi) > 0) {
                printf("Received packet...\n");
                if (!crc_ok) {
                    printf("crc not ok\n");
                }
                printf("lqi: ");
                printf("%d ",toLqi(lqi));
                printf("rssi: ");
                printf("%d ",toRssi(rssi));
                printf("dBm\n");
    
                if (crc_ok && length > 0) {
                    printf("packet: len ");
                    printf("%d\n",length);
                    printf("data: \n");
                    printf("%s\n",(const char *) packet);
                }
            }
    	    PinInputInterruptEnable(GD0_PORT,GD0_PIN);
        }
        now++;
        if (now > lastSend + sendDelay) {
    	    PinInputInterruptDisable(GD0_PORT,GD0_PIN);
    
            lastSend = now;
            const char *message = "hello world HC11";
            // We also need to include the 0 byte at the end of the string
            length = strlen(message)  + 1;
            strncpy((char *) packet, message, length);
    
            cc1101_sendData(packet,length);
            printf("Sent packet...");
            PinInputInterruptEnable(GD0_PORT,GD0_PIN);
        }
    }
}
