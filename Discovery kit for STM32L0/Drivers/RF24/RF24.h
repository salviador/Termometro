#ifndef __RF24_H__
#define __RF24_H__

#include "RF24_config.h"
#include "spiRF24.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/
  
typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;

typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;

typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;


class RF24
{
  private:
    bool p_variant; /* False for RF24L01 and true for RF24L01P */
    uint8_t payload_size; /**< Fixed size of payloads */
    bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
    uint8_t pipe0_reading_address[5]; /**< Last address set on pipe 0 for reading. */
    uint8_t addr_width; /**< The address width to use - 3,4 or 5 bytes. */
    uint8_t spi_rxbuff[32+1] ; //SPI receive buffer (payload max 32 bytes)
    uint8_t spi_txbuff[32+1] ; //SPI transmit buffer (payload max 32 bytes + 1 byte for the command)


    
    uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
    uint8_t write_register(uint8_t reg, uint8_t value);
    uint8_t read_register(uint8_t reg);
    uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
   
    void toggle_features(void);
    uint8_t spiTrans(uint8_t cmd);
    void closeReadingPipe( uint8_t pipe );
    uint8_t get_status(void);
    uint8_t read_payload(void* buf, uint8_t data_len);
    void print_status(uint8_t status);
    void print_address_register(const char* name, uint8_t reg, uint8_t qty = 1);
    void print_byte_register(const char* name, uint8_t reg, uint8_t qty = 1);
    rf24_datarate_e getDataRate( void );
    bool isPVariant(void);
    rf24_crclength_e getCRCLength(void);
    uint8_t getPALevel(void);

    
  public:    
    RF24(void);
    bool begin(void);
    void setRetries(uint8_t delay, uint8_t count);
    bool setDataRate(rf24_datarate_e speed);
    void setChannel(uint8_t channel);
    uint8_t flush_rx(void);
    uint8_t flush_tx(void);
    void powerUp(void);
    void setPALevel(uint8_t level);
    void openWritingPipe(uint64_t value);
    void openWritingPipe(const uint8_t *address);
    void openReadingPipe(uint8_t child, uint64_t address);
    void openReadingPipe(uint8_t child, const uint8_t *address);
    void startListening(void);
    void stopListening(void);
    bool write( const void* buf, uint8_t len );
    bool write( const void* buf, uint8_t len, const bool multicast );
    void startFastWrite( const void* buf, uint8_t len, const bool multicast);
    uint8_t write_payload(const void* buf, uint8_t data_len, const uint8_t writeType);
    bool available(void);
    bool available(uint8_t* pipe_num);
    void read( void* buf, uint8_t len );
    void setAutoAck(bool enable);
    void enableAckPayload(void);
    void setPayloadSize(uint8_t size);
    void printDetails(void);
    void enableDynamicPayloads(void);
    uint8_t getDynamicPayloadSize(void);
    void powerDown(void);

    
    uint32_t txDelay;

};

/*
#ifdef __cplusplus
}
#endif
*/

#endif // __RF24_H__

