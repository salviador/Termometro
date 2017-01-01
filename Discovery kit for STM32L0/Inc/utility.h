#ifndef UTILITY_H
#define UTILITY_H

#include "stm32l0xx_hal.h"
#include <stdlib.h>
#include "esp8266.h"
#include "RF24.h"
/* C++ detection */




#ifdef __cplusplus
extern "C" {
#endif

  
struct DATATIME{
  uint8_t giorno;
  uint8_t mese;
  uint8_t anno;
  uint8_t ora;
  uint8_t minuti;
  uint8_t secondi;
};
struct RecivePlayload{
  float Temp;
  float Hum;
  float Battery;  
};

struct Tempminmax{
  float TempMax;
  DATATIME tMax;
  uint8_t valid_max;
  float TempMin;
  DATATIME tMin;  
  uint8_t valid_min;
};

    uint8_t get_data_time_in_server(ESP8266 esp);
    void go_stop(uint8_t v);
    void get_radio(RF24 radio);
    void update_lcd(RecivePlayload rxdata, Tempminmax tminmax, DATATIME tnow);
    void SystemPower_Config_OFF(void);  
    uint8_t update_temp_toServer(ESP8266 esp, DATATIME *timenow, RecivePlayload *recivePlayload, Tempminmax *tminmax);
    uint8_t _get_data_time_in_server(ESP8266 esp);

  
  
  
  
  
  
  
  
  
  
/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
