#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "esp8266_ll.h"

class ESP8266
{

public:
  ESP8266(void);
  void begin(void);

  void send_command(char const *  data);
  uint16_t available(void);
  uint8_t wait_response_ReadToken(uint8_t *OUTbuffer,uint16_t lenOUTbuffer,const uint8_t *TokenBuffer,uint8_t lenTokenBuffer,uint32_t TimeOutms);
  void ESP8266::flush_ALL(void);

private:
  uint8_t ESP8266::read_byte(void);
  void ESP8266::flush_RX(void);
  void ESP8266::flush_TX(void);







};

#endif // __RF24_H__

