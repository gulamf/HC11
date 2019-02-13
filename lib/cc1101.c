#include <stdint.h>
#include "stm8s.h"
#include "delay.h"
#include "gpio.h"
#include "cc1101.h"
#include "spi.h"


/*
* RF state
*/
uint8_t cc1101_rfState ;


/**
* Carrier frequency
*/
uint8_t cc1101_carrierFreq = CFREQ_868;

/**
* Working mode (speed, ...)
*/
uint8_t cc1101_workMode = MODE_LOW_SPEED;

/**
* Frequency channel
*/
uint8_t cc1101_channel = CC1101_DEFVAL_CHANNR;

/**
* Synchronization word
*/
uint8_t cc1101_syncWord[2] = {CC1101_DEFVAL_SYNC1,CC1101_DEFVAL_SYNC0};

/**
* Device address
*/
uint8_t cc1101_devAddress = CC1101_DEFVAL_ADDR;


const char *marcState[23] = { "SLEEP", "IDLE", "XOFF", "VCOON_MC", "REGON_MC", "MANCAL", "VCOON", "REGON", "STARTCAL", "BWBOOST", "FS_LOCK","IFADCON","ENDCAL","RX","RX_END","RX_RST", "TXRX_SWITCH","RXFIFO_OVERFLOW","FSTXON","TX","TX_END","RXTX_SWITCH","TXFIFO_UNDERFLOW"};



void cc1101_init(uint8_t freq, uint8_t mode) {

	// set CSN_PIN as output pin 
	PinOutput(CSN_PORT,CSN_PIN);
	PinOutputPullUp(CSN_PORT,CSN_PIN);
	PinOutputSpeed2MHZ(CSN_PORT,CSN_PIN);


	PinInput(GD0_PORT,GD0_PIN);
	PinInputFloat(GD0_PORT,GD0_PIN);

	PinInput(SO_PORT,SO_PIN);		//GD1
	PinInputFloat(SO_PORT,SO_PIN);		//GD1

	PinInput(GD2_PORT,GD2_PIN);
	PinInputFloat(GD2_PORT,GD2_PIN);

	PinOutput(SI_PORT,SI_PIN);
	PinOutputPullUp(SI_PORT,SI_PIN);
	PinOutputSpeed2MHZ(SI_PORT,SI_PIN);

	cc1101_carrierFreq = freq;
	cc1101_workMode = mode;
	SPI_init();		                      
	cc1101_reset();
	cc1101_setTxPowerAmp(PA_LowPower);
}

void cc1101_waitReady() {
	while(PinRead(SO_PORT,SO_PIN)); // wait for CC1101 to become ready
}

void cc1101_select() {
	PinLow(CSN_PORT,CSN_PIN);
	cc1101_waitReady();
}

void cc1101_unSelect() {
	PinHigh(CSN_PORT,CSN_PIN);
}

void wait_GDO0_high() {
	while (!PinRead(GD0_PORT,GD0_PIN));
}

void wait_GDO0_low() {
	while(PinRead(GD0_PORT,GD0_PIN));
}

uint8_t cc1101_readReg(uint8_t address, uint8_t regType) {
	uint8_t rv;
	cc1101_select();
	SPI_write(address | regType);
	rv = SPI_write(0x00); // read actual value;
	cc1101_unSelect();
	return  rv;
}


uint8_t cc1101_readSingle(uint8_t address){
	uint8_t rv;
	cc1101_select();
	SPI_write(address | READ_SINGLE);
	rv = SPI_write(0x00); // read actual value;
	cc1101_unSelect();
	return  rv;
}

uint8_t cc1101_readStatus(uint8_t address){
	uint8_t rv;
	cc1101_select();
	SPI_write(address | READ_BURST);
	rv = SPI_write(0x00); // read actual value;
	cc1101_unSelect();
	return  rv;
}

void cc1101_writeSingle(uint8_t address, uint8_t value) {
	cc1101_select();
	SPI_write(address);
	SPI_write(value);	
	cc1101_unSelect();
}


void cc1101_readBurst(uint8_t address, uint8_t *buff, uint8_t len) {
	int i;
	cc1101_select();
	SPI_write(address | READ_BURST);
	for(i=0;i<len;i++) {
		buff[i] = SPI_write(0x00);
	}
	cc1101_unSelect();
}

void cc1101_writeBurst(uint8_t address, uint8_t *buff, uint8_t len) {
	int i;
	cc1101_select();
	SPI_write(address | WRITE_BURST);
	for(i=0;i<len;i++) {
		SPI_write(buff[i]);	
	}
	cc1101_unSelect();
}

void cc1101_writeCmdStrobe(uint8_t command ){
	cc1101_select();
	SPI_write(command);
	cc1101_unSelect();
}

void cc1101_reset() {
	PinHigh(CSN_PORT,CSN_PIN);
	_delay_us(5);
	PinLow(CSN_PORT,CSN_PIN);
	_delay_us(10);
	PinHigh(CSN_PORT,CSN_PIN);
	_delay_us(41);
  	cc1101_select();
	cc1101_writeCmdStrobe(CC1101_SRES);
	cc1101_unSelect();
  	cc1101_setCCregs();

}

void cc1101_setTxPowerAmp(uint8_t level) {
    cc1101_writeSingle(CC1101_PATABLE, level);
}

void cc1101_setCCregs() {
  cc1101_writeSingle(CC1101_IOCFG2,  CC1101_DEFVAL_IOCFG2);
  cc1101_writeSingle(CC1101_IOCFG1,  CC1101_DEFVAL_IOCFG1);
  cc1101_writeSingle(CC1101_IOCFG0,  CC1101_DEFVAL_IOCFG0);
  cc1101_writeSingle(CC1101_FIFOTHR,  CC1101_DEFVAL_FIFOTHR);
  cc1101_writeSingle(CC1101_PKTLEN,  CC1101_DEFVAL_PKTLEN);
  cc1101_writeSingle(CC1101_PKTCTRL1,  CC1101_DEFVAL_PKTCTRL1);
  cc1101_writeSingle(CC1101_PKTCTRL0,  CC1101_DEFVAL_PKTCTRL0);

  // Set default synchronization word
  cc1101_setSyncWord(cc1101_syncWord[0], cc1101_syncWord[1]);

  // Set default device address
  cc1101_setDevAddress(cc1101_devAddress);

  // Set default frequency channel
  cc1101_setChannel(cc1101_channel);
  
  cc1101_writeSingle(CC1101_FSCTRL1,  CC1101_DEFVAL_FSCTRL1);
  cc1101_writeSingle(CC1101_FSCTRL0,  CC1101_DEFVAL_FSCTRL0);

  // Set default carrier frequency = 868 MHz
  cc1101_setCarrierFreq(cc1101_carrierFreq);

  // RF speed
  if (cc1101_workMode == MODE_LOW_SPEED)
    cc1101_writeSingle(CC1101_MDMCFG4,  CC1101_DEFVAL_MDMCFG4_4800);
  else
    cc1101_writeSingle(CC1101_MDMCFG4,  CC1101_DEFVAL_MDMCFG4_38400);
    
  cc1101_writeSingle(CC1101_MDMCFG3,  CC1101_DEFVAL_MDMCFG3);
  cc1101_writeSingle(CC1101_MDMCFG2,  CC1101_DEFVAL_MDMCFG2);
  cc1101_writeSingle(CC1101_MDMCFG1,  CC1101_DEFVAL_MDMCFG1);
  cc1101_writeSingle(CC1101_MDMCFG0,  CC1101_DEFVAL_MDMCFG0);
  cc1101_writeSingle(CC1101_DEVIATN,  CC1101_DEFVAL_DEVIATN);
  cc1101_writeSingle(CC1101_MCSM2,  CC1101_DEFVAL_MCSM2);
  cc1101_writeSingle(CC1101_MCSM1,  CC1101_DEFVAL_MCSM1);
  cc1101_writeSingle(CC1101_MCSM0,  CC1101_DEFVAL_MCSM0);
  cc1101_writeSingle(CC1101_FOCCFG,  CC1101_DEFVAL_FOCCFG);
  cc1101_writeSingle(CC1101_BSCFG,  CC1101_DEFVAL_BSCFG);
  cc1101_writeSingle(CC1101_AGCCTRL2,  CC1101_DEFVAL_AGCCTRL2);
  cc1101_writeSingle(CC1101_AGCCTRL1,  CC1101_DEFVAL_AGCCTRL1);
  cc1101_writeSingle(CC1101_AGCCTRL0,  CC1101_DEFVAL_AGCCTRL0);
  cc1101_writeSingle(CC1101_WOREVT1,  CC1101_DEFVAL_WOREVT1);
  cc1101_writeSingle(CC1101_WOREVT0,  CC1101_DEFVAL_WOREVT0);
  cc1101_writeSingle(CC1101_WORCTRL,  CC1101_DEFVAL_WORCTRL);
  cc1101_writeSingle(CC1101_FREND1,  CC1101_DEFVAL_FREND1);
  cc1101_writeSingle(CC1101_FREND0,  CC1101_DEFVAL_FREND0);
  cc1101_writeSingle(CC1101_FSCAL3,  CC1101_DEFVAL_FSCAL3);
  cc1101_writeSingle(CC1101_FSCAL2,  CC1101_DEFVAL_FSCAL2);
  cc1101_writeSingle(CC1101_FSCAL1,  CC1101_DEFVAL_FSCAL1);
  cc1101_writeSingle(CC1101_FSCAL0,  CC1101_DEFVAL_FSCAL0);
  cc1101_writeSingle(CC1101_RCCTRL1,  CC1101_DEFVAL_RCCTRL1);
  cc1101_writeSingle(CC1101_RCCTRL0,  CC1101_DEFVAL_RCCTRL0);
  cc1101_writeSingle(CC1101_FSTEST,  CC1101_DEFVAL_FSTEST);
  cc1101_writeSingle(CC1101_PTEST,  CC1101_DEFVAL_PTEST);
  cc1101_writeSingle(CC1101_AGCTEST,  CC1101_DEFVAL_AGCTEST);
  cc1101_writeSingle(CC1101_TEST2,  CC1101_DEFVAL_TEST2);
  cc1101_writeSingle(CC1101_TEST1,  CC1101_DEFVAL_TEST1);
  cc1101_writeSingle(CC1101_TEST0,  CC1101_DEFVAL_TEST0);
  
  cc1101_sendData("",0);

}


void cc1101_setSyncWord(uint8_t syncH, uint8_t syncL) 
{
  cc1101_writeSingle(CC1101_SYNC1, syncH);
  cc1101_writeSingle(CC1101_SYNC0, syncL);
  cc1101_syncWord[0] = syncH;
  cc1101_syncWord[1] = syncL;
}

void cc1101_setDevAddress(uint8_t addr) 
{
  cc1101_writeSingle(CC1101_ADDR, addr);
  cc1101_devAddress = addr;
}

void cc1101_setChannel(uint8_t chnl) 
{
  cc1101_writeSingle(CC1101_CHANNR,  chnl);
  cc1101_channel = chnl;
}

void cc1101_setCarrierFreq(uint8_t freq)
{
  switch(freq)
  {
    case CFREQ_915:
      cc1101_writeSingle(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_915);
      cc1101_writeSingle(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_915);
      cc1101_writeSingle(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_915);
      break;
    case CFREQ_433:
      cc1101_writeSingle(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_433);
      cc1101_writeSingle(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_433);
      cc1101_writeSingle(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_433);
      break;
    case CFREQ_918:
      cc1101_writeSingle(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_918);
      cc1101_writeSingle(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_918);
      cc1101_writeSingle(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_918);
      break;
    default:
      cc1101_writeSingle(CC1101_FREQ2,  CC1101_DEFVAL_FREQ2_868);
      cc1101_writeSingle(CC1101_FREQ1,  CC1101_DEFVAL_FREQ1_868);
      cc1101_writeSingle(CC1101_FREQ0,  CC1101_DEFVAL_FREQ0_868);
      break;
  }
   
  cc1101_carrierFreq = freq;  
}

void cc1101_setPowerDownState() 
{
  cc1101_writeCmdStrobe(CC1101_SIDLE);
  cc1101_writeCmdStrobe(CC1101_SPWD);
}


uint8_t cc1101_sendData(uint8_t *buf, uint8_t len)
{
  uint8_t marcState;
  uint8_t res = 0;
  int tries = 0;
 
  cc1101_rfState = RFSTATE_TX;

  cc1101_setRxState();

  while (tries++ < 1000 && ((marcState = cc1101_readStatus(CC1101_MARCSTATE)) & 0x1F) != 0x0D)
  {
    if (marcState == 0x11)        // RX_OVERFLOW
      cc1101_flushRxFifo();              // flush receive queue
  }
  if (tries >= 1000) {
    // TODO: MarcState sometimes never enters the expected state; this is a hack workaround.
    return 0;
  }

  _delay_us(500);

  if (len > 0)
  {
    // Set data length at the first position of the TX FIFO
    cc1101_writeSingle(CC1101_TXFIFO,  len);
    // Write data into the TX FIFO
    cc1101_writeBurst(CC1101_TXFIFO, buf, len);

    // CCA enabled: will enter TX state only if the channel is clear
    cc1101_setTxState();
  }

  // Check that TX state is being entered (state = RXTX_SETTLING)
  marcState = cc1101_readStatus(CC1101_MARCSTATE) & 0x1F;
  if((marcState != 0x13) && (marcState != 0x14) && (marcState != 0x15))
  {
    cc1101_setIdleState();       // Enter IDLE state
    cc1101_flushTxFifo();        // Flush Tx FIFO
    cc1101_setRxState();         // Back to RX state

    // Declare to be in Rx state
    cc1101_rfState = RFSTATE_RX;
    return 0;
  }

  // Wait for the sync word to be transmitted
  wait_GDO0_high();

  // Wait until the end of the packet transmission
  wait_GDO0_low();

  // Check that the TX FIFO is empty
  if((cc1101_readStatus(CC1101_TXBYTES) & 0x7F) == 0)
    res = 1;

  cc1101_setIdleState();       // Enter IDLE state
  cc1101_flushTxFifo();        // Flush Tx FIFO

  // Enter back into RX state
  cc1101_setRxState();

  // Declare to be in Rx state
  cc1101_rfState = RFSTATE_RX;

  return res;
}

void cc1101_setTxState(void)
{
  cc1101_writeCmdStrobe(CC1101_STX);
  cc1101_rfState = RFSTATE_TX;
}

void cc1101_setRxState(void)
{
  cc1101_writeCmdStrobe(CC1101_SRX);
  cc1101_rfState = RFSTATE_RX;
}

void cc1101_flushRxFifo() { 
	cc1101_writeCmdStrobe(CC1101_SFRX);
}

void  cc1101_setIdleState()
{
	cc1101_writeCmdStrobe(CC1101_SIDLE);
}

void cc1101_flushTxFifo() {
	cc1101_writeCmdStrobe(CC1101_SFTX);
}


uint8_t cc1101_receiveData(uint8_t *buf, uint8_t *len, uint8_t *rssi, uint8_t *crc_ok, uint8_t *lqi)
{
  uint8_t val;
  uint8_t rxBytes = cc1101_readStatus(CC1101_RXBYTES);

  // Any byte waiting to be read and no overflow?
  if (rxBytes & 0x7F && !(rxBytes & 0x80))
  {
    // Read data length
    *len = cc1101_readSingle(CC1101_RXFIFO);
    // If packet is too long
    if (*len > DATA_LEN)
      *len = 0;   // Discard packet
    else
    {
      // Read data packet
      cc1101_readBurst(CC1101_RXFIFO, buf,  *len);
      // Read RSSI
      val = cc1101_readSingle(CC1101_RXFIFO);
      if(rssi) *rssi = val; 
      // Read LQI and CRC_OK
      val = cc1101_readSingle(CC1101_RXFIFO);
      if(lqi) *lqi = val & 0x7F;
      if(crc_ok) *crc_ok = val & 0x80;
    }
  }
  else
    *len = 0;

  cc1101_setIdleState();       // Enter IDLE state
  cc1101_flushRxFifo();        // Flush Rx FIFO
  //cmdStrobe(CC1101_SCAL);

  // Back to RX state
  cc1101_setRxState();

  return *len;
}


void cc1101_disableAddressCheck() {
	cc1101_writeSingle(CC1101_PKTCTRL1, 0x04);
}
void cc1101_enableAddressCheck() {
	cc1101_writeSingle(CC1101_PKTCTRL1, 0x06);
}
void cc1101_disableCCA() {
	cc1101_writeSingle(CC1101_MCSM1, 0);
}

void cc1101_enableCCA() {
	cc1101_writeSingle(CC1101_MCSM1, CC1101_DEFVAL_MCSM1);
}
