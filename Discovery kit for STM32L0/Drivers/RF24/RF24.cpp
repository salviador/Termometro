#include "nRF24L01.h"
#include "RF24_config.h"
#include "RF24.h"
#include <stdio.h>
#include <string.h>

#define _BV(x) (1<<(x))

static const uint8_t child_pipe[] =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_payload_size[] =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};
static const uint8_t child_pipe_enable[] =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};


RF24::RF24(void):
  p_variant(false),
  payload_size(32), dynamic_payloads_enabled(false), addr_width(5)//,pipe0_reading_address(0)
{
  pipe0_reading_address[0]=0;
}



bool RF24::begin(void)
{
  uint8_t setup=0;
  spi_RF24_init();
  gpio_RF24_init();
  
  HAL_Delay(5);
  

  
  
  write_register(NRF_CONFIG, 0x0C );
  
  setRetries(5,15);

  if( setDataRate( RF24_250KBPS ) )
  {
    p_variant = true ;
  }
  setup = read_register(RF_SETUP);

  setDataRate( RF24_1MBPS ) ;
  
  toggle_features();
  write_register(FEATURE,0 );
  write_register(DYNPD,0);
  write_register(NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  setChannel(76);
  
  flush_rx();
  flush_tx();

  powerUp();
  
  write_register(NRF_CONFIG, ( read_register(NRF_CONFIG) ) & ~_BV(PRIM_RX) );
 
  //write_register(NRF_CONFIG, 0x0F );
 
  // if setup is 0 or ff then there was no response from module
  return ( setup != 0 && setup != 0xff );
  
}  



uint8_t RF24::getDynamicPayloadSize(void)
{
  uint8_t result = 0;
  spi_txbuff[0] = R_RX_PL_WID;
  spi_rxbuff[1] = 0xff;
  csn_RF24(LOW); 
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, 2);
  result = spi_rxbuff[1];  
  csn_RF24(HIGH);

  
  if(result > 32) { flush_rx(); HAL_Delay(2); return 0; }
  return result;
}

void RF24::enableDynamicPayloads(void)
{
  // Enable dynamic payload throughout the system

    //toggle_features();
    write_register(FEATURE,read_register(FEATURE) | _BV(EN_DPL) );

 // Enable dynamic payload on all pipes
  //
  // Not sure the use case of only having dynamic payload on certain
  // pipes, so the library does not support it.
  write_register(DYNPD,read_register(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

  dynamic_payloads_enabled = true;
}
void RF24::setPayloadSize(uint8_t size)
{
  payload_size = rf24_min(size,32);
}

void RF24::enableAckPayload(void)
{
    //toggle_features();
  write_register(FEATURE,read_register(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );
  write_register(DYNPD,read_register(DYNPD) | _BV(DPL_P1) | _BV(DPL_P0));
  dynamic_payloads_enabled = true;
}

void RF24::setAutoAck(bool enable)
{
  if ( enable )
    write_register(EN_AA, 0x3F);
  else
    write_register(EN_AA, 0);
}

uint8_t RF24::read_payload(void* buf, uint8_t data_len)
{
  uint8_t status;
  uint8_t* current = reinterpret_cast<uint8_t*>(buf);

  if(data_len > payload_size) data_len = payload_size;
  uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
  //printf("[Reading %u bytes %u blanks]",data_len,blank_len);
 
    csn_RF24(LOW); 
    uint8_t * prx = spi_rxbuff;
    uint8_t * ptx = spi_txbuff;
    uint8_t size;
    size = data_len + blank_len + 1; // Add register value to transmit buffer

	*ptx++ =  R_RX_PAYLOAD;
	while(--size) 
		*ptx++ = NOP;
		
	size = data_len + blank_len + 1; // Size has been lost during while, re affect
	
	spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, size);
        
	status = *prx++; // 1st byte is status	
    
    if (data_len > 0) {
      while ( --data_len ) // Decrement before to skip 1st status byte
          *current++ = *prx++;
		
      *current = *prx;
    }
	csn_RF24(HIGH);

  return status;
}

void RF24::read( void* buf, uint8_t len ){

  // Fetch the payload
  read_payload( buf, len );

  //Clear the two possible interrupt flags with one command
  write_register(NRF_STATUS,_BV(RX_DR) | _BV(MAX_RT) | _BV(TX_DS) );

}

bool RF24::available(uint8_t* pipe_num)
{
  if (!( read_register(FIFO_STATUS) & _BV(RX_EMPTY) )){

    // If the caller wants the pipe number, include that
    if ( pipe_num ){
	  uint8_t status = get_status();
      *pipe_num = ( status >> RX_P_NO ) & 0x07;
  	}
  	return 1;
  }
  return 0;
}

bool RF24::available(void)
{
  return available(NULL);
}

bool RF24::write( const void* buf, uint8_t len, const bool multicast )
{
	//Start Writing
	startFastWrite(buf,len,multicast);
	
	while( ! ( get_status()  & ( _BV(TX_DS) | _BV(MAX_RT) ))) { 
	}
   
	ce_RF24(LOW);

	uint8_t status = write_register(NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  //Max retries exceeded
  if( status & _BV(MAX_RT)){
  	flush_tx(); //Only going to be 1 packet int the FIFO at a time using this method, so just flush
  	return 0;
  }
	//TX OK 1 or 0
  return 1;
}

bool RF24::write( const void* buf, uint8_t len ){
	return write(buf,len,0);
}
void RF24::startFastWrite( const void* buf, uint8_t len, const bool multicast){ //TMRh20

	//write_payload( buf,len);
	write_payload( buf, len,multicast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD ) ;
        ce_RF24(HIGH);

}
uint8_t RF24::write_payload(const void* buf, uint8_t data_len, const uint8_t writeType)
{
  uint8_t status;
  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);

   data_len = rf24_min(data_len, payload_size);
   uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;
  
   
  csn_RF24(LOW); 
  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;
  uint8_t size;
  size = data_len + blank_len + 1 ; // Add register value to transmit buffer
  *ptx++ =  writeType;
  while ( data_len-- )
    *ptx++ =  *current++;
  while ( blank_len-- )
    *ptx++ =  0;

  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, size);
  
  status = *prx; // status is 1st byte of receive buffer
  csn_RF24(HIGH);

  return status;
}

void RF24::startListening(void)
{
  powerUp();
  write_register(NRF_CONFIG, read_register(NRF_CONFIG) | _BV(PRIM_RX));
  write_register(NRF_STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  ce_RF24(HIGH);
  // Restore the pipe0 adddress, if exists
  if (pipe0_reading_address[0] > 0){
    write_register(RX_ADDR_P0, pipe0_reading_address, addr_width);	
  }else{
	closeReadingPipe(0);
  }

  // Flush buffers
  //flush_rx();
  if(read_register(FEATURE) & _BV(EN_ACK_PAY)){
	flush_tx();
  }

  // Go!
  //delayMicroseconds(100);
}
void RF24::stopListening(void)
{  
  ce_RF24(LOW);

  HAL_Delay(txDelay);
  
  if(read_register(FEATURE) & _BV(EN_ACK_PAY)){
    HAL_Delay(txDelay); //200
	flush_tx();
  }
  //flush_rx();
  write_register(NRF_CONFIG, ( read_register(NRF_CONFIG) ) & ~_BV(PRIM_RX) );
 
  write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(child_pipe_enable[0])); // Enable RX on pipe0
  
  //delayMicroseconds(100);

}

void RF24::closeReadingPipe( uint8_t pipe )
{
  write_register(EN_RXADDR,read_register(EN_RXADDR) & ~_BV(child_pipe_enable[pipe]));
}
void RF24::openReadingPipe(uint8_t child, uint64_t address)
{
  if (child == 0){
    memcpy(pipe0_reading_address,&address,addr_width);
  }

  if (child <= 6)
  {
    // For pipes 2-5, only write the LSB
    if ( child < 2 )
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), addr_width);
    else
      write_register(child_pipe[child], reinterpret_cast<const uint8_t*>(&address), 1);

    write_register(child_payload_size[child],payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
    write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));
  }
}

void RF24::openReadingPipe(uint8_t child, const uint8_t *address)
{
  // If this is pipe 0, cache the address.  This is needed because
  // openWritingPipe() will overwrite the pipe 0 address, so
  // startListening() will have to restore it.
  if (child == 0){
    memcpy(pipe0_reading_address,address,addr_width);
  }
  if (child <= 6)
  {
    // For pipes 2-5, only write the LSB
    if ( child < 2 ){
      write_register(child_pipe[child], address, addr_width);
    }else{
      write_register(child_pipe[child], address, 1);
	}
    write_register(child_payload_size[child],payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
    write_register(EN_RXADDR,read_register(EN_RXADDR) | _BV(child_pipe_enable[child]));

  }
}

void RF24::openWritingPipe(uint64_t value)
{
  write_register(RX_ADDR_P0, reinterpret_cast<uint8_t*>(&value), addr_width);
  write_register(TX_ADDR, reinterpret_cast<uint8_t*>(&value), addr_width);
  write_register(RX_PW_P0,payload_size);
}

/****************************************************************************/
void RF24::openWritingPipe(const uint8_t *address)
{
  write_register(RX_ADDR_P0,address, addr_width);
  write_register(TX_ADDR, address, addr_width);
  write_register(RX_PW_P0,payload_size);
}


void RF24::setPALevel(uint8_t level)
{

  uint8_t setup = read_register(RF_SETUP) & 0xF8;

  if(level > 3){  						// If invalid level, go to max PA
	  level = (RF24_PA_MAX << 1) + 1;		// +1 to support the SI24R1 chip extra bit
  }else{
	  level = (level << 1) + 1;	 		// Else set level as requested
  }


  write_register( RF_SETUP, setup |= level ) ;	// Write it to the chip
}

void RF24::powerUp(void)
{
   uint8_t cfg = read_register(NRF_CONFIG);

   // if not powered up then power up and wait for the radio to initialize
   if (!(cfg & _BV(PWR_UP))){
      write_register(NRF_CONFIG, cfg | _BV(PWR_UP));

      // For nRF24L01+ to go from power down mode to TX or RX mode it must first pass through stand-by mode.
	  // There must be a delay of Tpd2stby (see Table 16.) after the nRF24L01+ leaves power down mode before
	  // the CEis set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
      HAL_Delay(5);
   }
}
void RF24::powerDown(void)
{
 ce_RF24(LOW);
  write_register(NRF_CONFIG,read_register(NRF_CONFIG) & ~_BV(PWR_UP));
}

uint8_t RF24::spiTrans(uint8_t cmd){
  uint8_t status;
   csn_RF24(LOW);

  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;
  *ptx = cmd;	
  
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, 1);
 
  status = *prx++; // status is 1st byte of receive buffer
  
   csn_RF24(HIGH);
  return status;
}
uint8_t RF24::flush_rx(void)
{
  return spiTrans( FLUSH_RX );
}
uint8_t RF24::flush_tx(void)
{
  return spiTrans( FLUSH_TX );
}

void RF24::setChannel(uint8_t channel)
{
  const uint8_t max_channel = 125;
  write_register(RF_CH,rf24_min(channel,max_channel));
}

void RF24::toggle_features(void)
{/*
  csn_RF24(LOW);
    _SPI.transfer( ACTIVATE); );
    _SPI.transfer( 0x73 );
  csn_RF24(HIGH);
  */
  write_register(ACTIVATE, 0x73);
}

bool RF24::setDataRate(rf24_datarate_e speed)
{
  bool result = false;
  uint8_t setup = read_register(RF_SETUP) ;

  // HIGH and LOW '00' is 1Mbs - our default
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
  
  txDelay=85;
  
  if( speed == RF24_250KBPS )
  {
    // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
    // Making it '10'.
    setup |= _BV( RF_DR_LOW ) ;
    txDelay=155;
  }
  else
  {
    // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
    // Making it '01'
    if ( speed == RF24_2MBPS )
    {
      setup |= _BV(RF_DR_HIGH);
	  txDelay=65;
    }
  }
  write_register(RF_SETUP,setup);

  // Verify our result
  if ( read_register(RF_SETUP) == setup )
  {
    result = true;
  }
  return result;
}

void RF24::setRetries(uint8_t delay, uint8_t count)
{
 write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}


uint8_t RF24::read_register(uint8_t reg, uint8_t* buf, uint8_t len)
{
  uint8_t status;

  csn_RF24(LOW);
  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;
  uint8_t size = len + 1; // Add register value to transmit buffer

  *ptx++ = ( R_REGISTER | ( REGISTER_MASK & reg ) );

  while (len--){ *ptx++ = NOP; } // Dummy operation, just for reading
  
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, size);
  
  status = *prx++; // status is 1st byte of receive buffer

  // decrement before to skip status byte
  while ( --size ){ *buf++ = *prx++; } 
  csn_RF24(HIGH);
  
  return status;
}


uint8_t RF24::read_register(uint8_t reg)
{
  uint8_t result;
  csn_RF24(LOW); 
  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;	
  *ptx++ = ( R_REGISTER | ( REGISTER_MASK & reg ) );
  *ptx++ = NOP ; // Dummy operation, just for reading
  
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, 2);
  
  result = *++prx;   // result is 2nd byte of receive buffer
  csn_RF24(HIGH);
  
  return result;
}

uint8_t RF24::write_register(uint8_t reg, const uint8_t* buf, uint8_t len)
{
  uint8_t status;
  csn_RF24(LOW); 
  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;
  uint8_t size = len + 1; // Add register value to transmit buffer

  *ptx++ = ( W_REGISTER | ( REGISTER_MASK & reg ) );
  while ( len-- )
    *ptx++ = *buf++;
  
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, size);
    
  status = *prx; // status is 1st byte of receive buffer
  csn_RF24(HIGH);
  return status;
}

uint8_t RF24::write_register(uint8_t reg, uint8_t value)
{
  uint8_t status;
  csn_RF24(LOW); 
  uint8_t * prx = spi_rxbuff;
  uint8_t * ptx = spi_txbuff;
  *ptx++ = ( W_REGISTER | ( REGISTER_MASK & reg ) );
  *ptx = value ;	
  
  spi_RF24_tx( (uint8_t *) spi_txbuff, (uint8_t *) spi_rxbuff, 2);
 
  status = *prx++; // status is 1st byte of receive buffer
  csn_RF24(HIGH);
  return status;
}

uint8_t RF24::get_status(void)
{
  return spiTrans(NOP);
}
bool RF24::isPVariant(void)
{
  return p_variant ;
}

rf24_datarate_e RF24::getDataRate( void )
{
  rf24_datarate_e result ;
  uint8_t dr = read_register(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));

  // switch uses RAM (evil!)
  // Order matters in our case below
  if ( dr == _BV(RF_DR_LOW) )
  {
    // '10' = 250KBPS
    result = RF24_250KBPS ;
  }
  else if ( dr == _BV(RF_DR_HIGH) )
  {
    // '01' = 2MBPS
    result = RF24_2MBPS ;
  }
  else
  {
    // '00' = 1MBPS
    result = RF24_1MBPS ;
  }
  return result ;
}
static const char rf24_pa_dbm_e_str_0[]  = "PA_MIN";
static const char rf24_pa_dbm_e_str_1[]  = "PA_LOW";
static const char rf24_pa_dbm_e_str_2[]  = "PA_HIGH";
static const char rf24_pa_dbm_e_str_3[]  = "PA_MAX";
static const char * const rf24_pa_dbm_e_str_P[]  = {
  rf24_pa_dbm_e_str_0,
  rf24_pa_dbm_e_str_1,
  rf24_pa_dbm_e_str_2,
  rf24_pa_dbm_e_str_3,
};

rf24_crclength_e RF24::getCRCLength(void)
{
  rf24_crclength_e result = RF24_CRC_DISABLED;
  
  uint8_t config = read_register(NRF_CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;
  uint8_t AA = read_register(EN_AA);
  
  if ( config & _BV(EN_CRC ) || AA)
  {
    if ( config & _BV(CRCO) )
      result = RF24_CRC_16;
    else
      result = RF24_CRC_8;
  }

  return result;
}

void RF24::print_status(uint8_t status)
{
  printf("STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n",
           status,
           (status & _BV(RX_DR))?1:0,
           (status & _BV(TX_DS))?1:0,
           (status & _BV(MAX_RT))?1:0,
           ((status >> RX_P_NO) & 0x07),
           (status & _BV(TX_FULL))?1:0
          );
}
uint8_t RF24::getPALevel(void)
{

  return (read_register(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH))) >> 1 ;
}
void RF24::print_address_register(const char* name, uint8_t reg, uint8_t qty)
{
  printf("%s\t =",name);
  while (qty--)
  {
    uint8_t buffer[5];
    read_register(reg++,buffer,sizeof buffer);

    printf(" 0x");
    uint8_t* bufptr = buffer + sizeof buffer;
    while( --bufptr >= buffer )
      printf("%02x",*bufptr);
  }

  printf("\r\n");
}
void RF24::print_byte_register(const char* name, uint8_t reg, uint8_t qty)
{
  //char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  //printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);
  printf("%s\t =", name);
  while (qty--)
    printf(" 0x%02x",read_register(reg++));
  printf("\r\n");
}

static const char rf24_datarate_e_str_0[]  = "1MBPS";
static const char rf24_datarate_e_str_1[]  = "2MBPS";
static const char rf24_datarate_e_str_2[]  = "250KBPS";
const char * rf24_datarate_e_str_P[] = {
  rf24_datarate_e_str_0,
  rf24_datarate_e_str_1,
  rf24_datarate_e_str_2,
};
static const char rf24_model_e_str_0[] = "nRF24L01";
static const char rf24_model_e_str_1[] = "nRF24L01+";
static const char * const rf24_model_e_str_P[] = {
  rf24_model_e_str_0,
  rf24_model_e_str_1,
};
static const char rf24_crclength_e_str_0[] = "Disabled";
static const char rf24_crclength_e_str_1[] = "8 bits";
static const char rf24_crclength_e_str_2[] = "16 bits" ;
static const char * const rf24_crclength_e_str_P[] = {
  rf24_crclength_e_str_0,
  rf24_crclength_e_str_1,
  rf24_crclength_e_str_2,
};

void RF24::printDetails(void)
{
  print_status(get_status());
  
  print_address_register("RX_ADDR_P0-1",RX_ADDR_P0,2);
  print_byte_register("RX_ADDR_P2-5",RX_ADDR_P2,4);
  print_address_register("TX_ADDR\t",TX_ADDR);
  
  print_byte_register("RX_PW_P0-6",RX_PW_P0,6);
  print_byte_register("EN_AA\t",EN_AA);
  print_byte_register("EN_RXADDR",EN_RXADDR);
  print_byte_register("RF_CH\t",RF_CH);
  print_byte_register("RF_SETUP",RF_SETUP);
  print_byte_register("CONFIG\t",NRF_CONFIG);
  print_byte_register("DYNPD/FEATURE",DYNPD,2);
 
  printf("Data Rate\t = %s\r\n",rf24_datarate_e_str_P[getDataRate()]);
  printf("Model\t\t = %s\r\n",rf24_model_e_str_P[isPVariant()]);  
  printf("CRC Length\t = %s\r\n",rf24_crclength_e_str_P[getCRCLength()]);
  printf("PA Power\t = %s\r\n",  rf24_pa_dbm_e_str_P[getPALevel()]); 
  
}