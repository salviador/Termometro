#include "utility.h"
#include "main.h"
#include "bsp.h"
#include "spiRF24.h"
#include "RF24.h"
#include <stdio.h>
#include <string.h>

volatile DATATIME datatime;
extern RTC_HandleTypeDef hrtc;
extern uint64_t pipes[2] =  { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
extern RecivePlayload recivePlayload; 

void go_stop(uint8_t v){

    //  resolution = 16/39000 = 0.000410256 s
    //  value a ms = 0.001 / 000410256 = 2.43750243750

    //  value_counter = 2.43750243750 * mS
    //  value_counter = 2437.50243750 * S

    //  value_counter < 65535  => max [26886 mS] => [26.8 S]
    
    
    //Test STOP MODE --> WAKE UP rtc_wakeup ~10sec
  // Disable Wakeup Counter 
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
   
  // Enable Ultra low power mode 
  HAL_PWREx_EnableUltraLowPower();
  // Enable the fast wake up from Ultra low power mode 
  HAL_PWREx_EnableFastWakeUp();
  // Select HSI as system clock source after Wake Up from Stop mode 
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
    // Check and handle if the system was resumed from StandBy mode 
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    // Clear Standby flag 
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
  }


    SystemPower_Config_OFF();

    if(v==0){
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);    
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);   
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
      HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
      HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    }
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 57813, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //25-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 28800, RTC_WAKEUPCLOCK_RTCCLK_DIV16);      //12-Sec  
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);    
    
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

}

void update_lcd(RecivePlayload rxdata, Tempminmax tminmax, DATATIME tnow){
  char bufferLCD[20];

  BSP_EPD_Clear(EPD_COLOR_WHITE);
  BSP_EPD_DrawHLine(0,9,75);
  BSP_EPD_DrawHLine(100,9,172);
  BSP_EPD_DrawVLine(86,0,8);
  BSP_EPD_SetFont(&Font8);   
  sprintf(&bufferLCD[0],"%.1fv",rxdata.Battery);
  BSP_EPD_DisplayStringAt(78, 8, (uint8_t *) &bufferLCD[0], LEFT_MODE);

  BSP_EPD_SetFont(&Font12);
  BSP_EPD_DisplayStringAt(30, 5, (uint8_t*)"MIN", LEFT_MODE);
  BSP_EPD_DisplayStringAt(120, 5, (uint8_t*)"MAX", LEFT_MODE);

  BSP_EPD_SetFont(&Font12);
  
  if(tminmax.valid_min==1){
    sprintf(&bufferLCD[0],"%.1f",tminmax.TempMin);
    BSP_EPD_DisplayStringAt(18, 0, (uint8_t *) &bufferLCD[0], LEFT_MODE);
  }else{
    BSP_EPD_DisplayStringAt(18, 0, (uint8_t *) "--.-", LEFT_MODE);
  }

  //sprintf(&bufferLCD[0],"%u",stop_temp);
  //BSP_EPD_DisplayStringAt(18, 0, (uint8_t *) &bufferLCD[0], LEFT_MODE);
  if(tminmax.valid_max==1){
    sprintf(&bufferLCD[0],"%.1f",tminmax.TempMax);
    BSP_EPD_DisplayStringAt(115, 0, (uint8_t *) &bufferLCD[0], LEFT_MODE);
  }else{
    BSP_EPD_DisplayStringAt(115, 0, (uint8_t *) "--.-", LEFT_MODE);
  }
    
  BSP_EPD_DrawRect(56,2,2,1);
  BSP_EPD_DrawRect(146,2,2,1);


  BSP_EPD_SetFont(&Font8);   
  
  if(tminmax.valid_min==1){  
    sprintf(&bufferLCD[0],"%02d:%02d:%02d",tminmax.tMin.ora,tminmax.tMin.minuti,tminmax.tMin.secondi);
    BSP_EPD_DisplayStringAt(20, 3, (uint8_t *) &bufferLCD[0], LEFT_MODE);
  }else{
    BSP_EPD_DisplayStringAt(20, 3, (uint8_t *) "--:--:--", LEFT_MODE);
  }
  if(tminmax.valid_max==1){   
    sprintf(&bufferLCD[0],"%02d:%02d:%02d",tminmax.tMax.ora,tminmax.tMax.minuti,tminmax.tMax.secondi);  
    BSP_EPD_DisplayStringAt(110, 3, (uint8_t *) &bufferLCD[0], LEFT_MODE);
  }else{
    BSP_EPD_DisplayStringAt(110, 3, (uint8_t *) "--:--:--", LEFT_MODE);
  }


  BSP_EPD_SetFont(&Font8);  
  sprintf(&bufferLCD[0],"%02d:%02d:%02d",tnow.ora,tnow.minuti,tnow.secondi);  
  BSP_EPD_DisplayStringAt(68, 15,  (uint8_t *) &bufferLCD[0], LEFT_MODE);

  BSP_EPD_SetFont(&Font20);

  sprintf(&bufferLCD[0],"%.1f",rxdata.Temp);

  BSP_EPD_DisplayStringAt(15, 10, (uint8_t *)&bufferLCD[0] , LEFT_MODE);

  sprintf(&bufferLCD[0],"%.0f%",rxdata.Hum);

  BSP_EPD_DisplayStringAt(120, 10, (uint8_t *) &bufferLCD[0] , LEFT_MODE);
  BSP_EPD_DrawRect(72,13,2,1);

  BSP_EPD_RefreshDisplay();





}

void get_radio(RF24 radio){

  uint8_t rx_state;
  
  //MX_GPIO_Init();
  spi_RF24_init();
  Bsp_Init();
  Bsp_power_ON_radio();
  HAL_Delay(250);  
  spi_RF24_init();
  Bsp_Init();
  radio.begin();
  radio.powerUp();  
  radio.enableDynamicPayloads();
  radio.setChannel(90);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX); 
  radio.setRetries(5,15);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]); 
  radio.startListening();  

  rx_state = 1;
  
  do{
    if ( radio.available() ){
      int len = radio.getDynamicPayloadSize();
      if(len == 12){
        radio.read( &recivePlayload, sizeof(recivePlayload));
      }
      radio.stopListening();       
      radio.startListening();        
      
      rx_state = 0;
      radio.powerDown();
    }
  }while(rx_state);

}







//return 0 = NO ERROR
uint8_t get_data_time_in_server(ESP8266 esp){
  uint8_t temp;
  uint8_t otoken[400];
  uint8_t icountparse,last_carriege_Return_number,icountparse2;    
  uint8_t buffer_DATATIME[6];
  
  esp.begin();

    esp.send_command( (char const * ) "AT\r\n");
    temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,5000);
    esp.flush_ALL();
    if(temp==1){
      //OK
      temp = 1;
      if(temp==1){
        //OK      
        esp.send_command( (char const * ) "ATE0\r\n");
        temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,5000);
        esp.flush_ALL();              
        if(temp==1){
        //OK
          esp.send_command( (char const * ) "AT+CWMODE=1\r\n");
          temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,5000);
          esp.flush_ALL();              
          HAL_Delay(10);
          if(temp==1){
            esp.send_command( (char const * ) "AT+CWJAP=\"net\",\"arturo1111\"\r\n");
            temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n",4,25000);
            esp.flush_ALL();              
            if(temp==1){
              if(_get_data_time_in_server(esp)==0){                
                return 0;
              }else{
                return 1;
              }
            }
          return 1;
          }
        }      
        return 1;
      }
      return 1;
    }
    return 1;
}

uint8_t _get_data_time_in_server(ESP8266 esp){
  uint8_t temp;
  uint8_t otoken[400];
  uint8_t icountparse,last_carriege_Return_number,icountparse2;    
  uint8_t buffer_DATATIME[6];
    
  esp.send_command( (char const * ) "AT+CIPSTART=\"TCP\",\"mancinimichele.altervista.org\",80\r\n");
  temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"CONNECT\r\n\r\nOK\r\n",15,25000);
  esp.flush_ALL();         
  if(temp==1){
    esp.send_command( (char const * ) "AT+CIPSEND=74\r\n");
    temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n> ",6,25000);
    esp.flush_ALL();         
    if(temp==1){
      esp.send_command( (char const * ) "GET /devices/getdate.php HTTP/1.1\r\nHost: mancinimichele.altervista.org\r\n\r\n");
      temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],255,(const uint8_t* )"CLOSED\r\n> ",8,25000);
      esp.flush_ALL();         
      if(temp==1){
        //Now parse
        icountparse = 0;
        //trova ==0
        for(int i = 0; i < 255; i++){
          if(otoken[i] == 0x00){
            icountparse = i;
            break;
          }                  
        }
        icountparse2 = icountparse;
        //vai indietro di tot "\r\n"                 
        last_carriege_Return_number = 7;
        for(int i = icountparse; i >= 0; i--){
          if((otoken[i] == 0x0D) && (otoken[i+1] == 0x0A)){
            last_carriege_Return_number--;
            if(last_carriege_Return_number==0){
              icountparse2 = i;
              break;                      
            }                    
          }
        }                  
        if((icountparse2 != icountparse)&&(icountparse2 > 0)){
          //trovato
          icountparse2 = icountparse2 + 2;
          //parse giorno
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x2F){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.giorno = atoi((char const *)&buffer_DATATIME[0]);
          
          //parse mese
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x2F){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;                       
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.mese = atoi((char const *)&buffer_DATATIME[0]);
          
          //parse anno
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x20){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;                       
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.anno = atoi((char const *)&buffer_DATATIME[0]);
          
          //parse ora
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x3A){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;                       
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.ora = atoi((char const *)&buffer_DATATIME[0]);
          
          //parse minuti
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x3A){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;                       
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.minuti = atoi((char const *)&buffer_DATATIME[0]);
          
          //parse secondi
          icountparse = 0;
          for(int i = icountparse2; i < (icountparse2 + 3); i++){
            if(otoken[i] == 0x2F){
              buffer_DATATIME[icountparse] = 0x00;
              icountparse2 = i + 1;                       
              break;
            }
            buffer_DATATIME[icountparse++] = otoken[i];
          }
          datatime.secondi = atoi((char const *)&buffer_DATATIME[0]);

          
          esp.send_command((char const *) "AT+CIPCLOSE\r\n");
          temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n",4,1000);
          esp.flush_ALL();              
          if(temp==1){
            return 0;
          }
          //wowwww  I have date/time
          return 0;
        }
      return 1;                    
      }
    return 1;               
    }            
  return 1;             
  }
  return 1;      
}
  
void SystemPower_Config_OFF(void){


  GPIO_InitTypeDef GPIO_InitStructure;

  __HAL_RCC_USART1_FORCE_RESET();
  __HAL_RCC_USART1_RELEASE_RESET();
  
  HAL_NVIC_DisableIRQ(USART1_IRQn);
  
  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
  
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  
  spi_RF24_deinit();

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
 
}





//return 0 = NO ERROR
uint8_t update_temp_toServer(ESP8266 esp, DATATIME *timenow, RecivePlayload *recivePlayload, Tempminmax *tminmax){

  uint8_t temp;
  uint8_t otoken[400];
  uint8_t icountparse,last_carriege_Return_number,icountparse2;    
  uint8_t buffer_DATATIME[6];
  char trasmissione[200];
  
    esp.begin();

    esp.send_command( (char const * ) "AT\r\n");
    temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,500);
    esp.flush_ALL();
    if(temp==1){
      //OK
      temp = 1;
      if(temp==1){
        //OK      
        esp.send_command( (char const * ) "ATE0\r\n");
        temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,500);
        esp.flush_ALL();              
        if(temp==1){
        //OK
          esp.send_command( (char const * ) "AT+CWMODE=1\r\n");
          temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],20,(const uint8_t* )"OK\r\n",4,5000);
          esp.flush_ALL();              
          HAL_Delay(10);
          if(temp==1){
            esp.send_command( (char const * ) "AT+CWJAP=\"net\",\"arturo1111\"\r\n");
            temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n",4,25000);
            esp.flush_ALL();              
            if(temp==1){
              esp.send_command( (char const * ) "AT+CIPSTART=\"TCP\",\"mancinimichele.altervista.org\",80\r\n");
              temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"CONNECT\r\n\r\nOK\r\n",15,25000);
              esp.flush_ALL();         
              if(temp==1){              //********************************************************
                
                //format string
                sprintf((char *)&trasmissione[0],"GET /IOT_Temp/iot_extemp.php?DATATIME=%02d-%02d-%04d;%02d:%02d:%02d&TEMP=%.1f&MIN=%.1f&MAX=%.1f HTTP/1.1\r\nHost: mancinimichele.altervista.org\r\n\r\n",
                          timenow->giorno,timenow->mese, (2000 + timenow->anno),timenow->ora,timenow->minuti,timenow->secondi,recivePlayload->Temp,tminmax->TempMin, tminmax->TempMax);
                
                sprintf((char *)&otoken[0],"AT+CIPSEND=%d\r\n", strlen((char * )&trasmissione[0]));
                
                esp.send_command((char *)&otoken[0]);
                temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n> ",6,25000);
                esp.flush_ALL();         
                if(temp==1){
                  esp.send_command((char *)&trasmissione[0]);
                  temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],255,(const uint8_t* )"CLOSED\r\n> ",8,25000);
                  esp.flush_ALL();         
                  if(temp==1){
                    esp.send_command((char const *) "AT+CIPCLOSE\r\n");
                    temp = esp.wait_response_ReadToken((uint8_t*)&otoken[0],100,(const uint8_t* )"OK\r\n",4,1000);
                    esp.flush_ALL();              
                    if(_get_data_time_in_server(esp)==0){
                        return 0;
                    }
                    return 1;                    
                  }
                return 1;               
                }            
              return 1;             
              }
            return 1;           
            }
          return 1;
          }
        }      
        return 1;
      }
      return 1;
    }
    return 1;
}
