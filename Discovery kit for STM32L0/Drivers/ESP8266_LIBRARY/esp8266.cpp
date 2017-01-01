#include <stdio.h>
#include <string.h>
#include "esp8266.h"


extern uint8_t p_r;
extern uint8_t p_w;
extern uint8_t p_wt;
extern uint8_t ESP_BUFFER[BUFFER_RX_ESP8266];
extern UART_HandleTypeDef UartHandle;

ESP8266::ESP8266(void){


}

void ESP8266::begin(void){
  ESP_LL_Init_RESET();
  ESP_LL_SetReset(1);
  HAL_Delay(500);  
  ESP_LL_SetReset(0);
  HAL_Delay(2500);   
  ESP_LL_Init();
  HAL_Delay(5); 
}

void ESP8266::send_command(char const * data){

  ESP_LL_SendData((uint8_t*)data, strlen((char const *) data));
}

uint16_t ESP8266::available(void){
  return (p_w - p_r);
}

uint8_t ESP8266::read_byte(void){
  uint8_t value; 
  value = ESP_BUFFER[p_r++ % BUFFER_RX_ESP8266];
  return value;
}

void ESP8266::flush_RX(void){
  p_r = 0;
}
void ESP8266::flush_TX(void){
  p_w = 0;
   p_wt = 1;
  UartHandle.pRxBuffPtr = &ESP_BUFFER[0];
}
void ESP8266::flush_ALL(void){
  flush_RX();
  flush_TX();  
}
/*
  value = USART3_ReadToken((uint8_t*)&bufferGSM[0],50,"OK\r\n",4,30000);
  if(value!=1){
    //error
    USART3_flush();      
    return 0;//10;
  }        
*/

/*
  Legge la seriale fino a che non riceve il TOKEN e restituisce la stringa
  oppure va in timeout
  return 0 se nn trova niente
  return 1 se trova
  return 2 se va oltre il buffer
*/
uint8_t ESP8266::wait_response_ReadToken(uint8_t *OUTbuffer,uint16_t lenOUTbuffer, const uint8_t *TokenBuffer,uint8_t lenTokenBuffer,uint32_t TimeOutms){
  uint16_t count,icount;
  uint8_t value8,icntoken;
  uint32_t start,end;
  icount = 0;                  
  icntoken = 0;

  start = HAL_GetTick();
  end = start;
  
 while((end - start) < TimeOutms){ 
    count = available();  
    if(count>0){
      value8 = read_byte();
      OUTbuffer[icount] = value8;
      if(icount>=lenOUTbuffer){
        return 2;
      }
      if(value8 == TokenBuffer[icntoken]){
        icntoken++;
        if(icntoken>=lenTokenBuffer){
          icount++;
          OUTbuffer[icount] = 0x00;
        // USART3_flush();
          return 1;
        }
      }else{
        icntoken = 0;
        if(value8 == TokenBuffer[icntoken]){
          icntoken++;
          if(icntoken>=lenTokenBuffer){
            icount++;
            OUTbuffer[icount] = 0x00;
          //  USART3_flush();
            return 1;
          }
        }
      }      
      icount++;      
    }
    end = HAL_GetTick();
  }
  return 0;
}

