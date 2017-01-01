/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "bsp.h"
#include "stm32l0538_discovery_epd.h"

#include "spiRF24.h"
#include "RF24.h"
    
#include "esp8266.h"
#include "utility.h"

//temp **********    
volatile uint8_t temp;
volatile uint32_t start_temp,stop_temp;


extern UART_HandleTypeDef UartHandle;

extern DATATIME datatime;


struct Tempminmax tminmax;
struct DATATIME timenow;
struct DATATIME lasttime;
struct DATATIME last_ServerTime;

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
  
RTC_HandleTypeDef hrtc;
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_RTC_Init(void);



struct RecivePlayload recivePlayload; 

RF24 radio;
ESP8266 esp;

const uint64_t pipes[2] =  { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const uint8_t ORAMINUTI_TXSERVER[] = {10,11,12,13,14,15,16,17,18,19,20,21,22};
uint8_t index_ORAMINUTI_TXSERVER = 0;


int main(void)
{
  uint8_t rx_state;
  
  HAL_Init();
  SystemClock_Config();
  
  MX_GPIO_Init();
//  MX_RTC_Init();
  
  Bsp_Init();
    
  /* Disable Wakeup Counter */
  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
   
  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();
  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
  
  MX_RTC_Init();
  
  rx_state = 1;
  //-------------------------------------------
  BSP_EPD_Clear(EPD_COLOR_WHITE);
  BSP_EPD_RefreshDisplay();
  
  /*
  //debug
  while(1){
    HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);
  
    printf("%02d:%02d:%02d\r\n",sTime.Hours,sTime.Minutes,sTime.Seconds);
  
      HAL_Delay(1000);
  }
  */
  
  
  //**** Get Data/time dal server
  Bsp_power_ON_ESP();
  do{
    if(get_data_time_in_server(esp)==0){
      sTime.Hours = datatime.ora;
      sTime.Minutes = datatime.minuti;
      sTime.Seconds = datatime.secondi;
      sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
      sTime.StoreOperation = RTC_STOREOPERATION_RESET;
      HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
      sDate.WeekDay = RTC_WEEKDAY_MONDAY;
      sDate.Month = datatime.mese;
      sDate.Date = datatime.giorno;
      sDate.Year = datatime.anno;
      HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
      rx_state = 0;
    }else{
      HAL_Delay(5000);    
    }
  }while(rx_state==1);
  Bsp_power_OFF_ESP();
  
  
  index_ORAMINUTI_TXSERVER = 0;
       
  //------------------------------------
  
  //**** Aspetta RX nodo
  get_radio(radio);

  //*** Resetta min/max
  HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);
  tminmax.TempMax = -1000.0;
  tminmax.TempMin = 1000.0;
  tminmax.tMax.anno = sDate.Year;
  tminmax.tMax.mese = sDate.Month;
  tminmax.tMax.giorno = sDate.Date;
  tminmax.tMin.anno = sDate.Year;
  tminmax.tMin.mese = sDate.Month;
  tminmax.tMin.giorno = sDate.Date;  
  HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);    
  tminmax.tMax.ora = sTime.Hours;
  tminmax.tMax.minuti = sTime.Minutes;
  tminmax.tMax.secondi = sTime.Seconds;
  tminmax.tMin.ora = sTime.Hours;
  tminmax.tMin.minuti = sTime.Minutes;
  tminmax.tMin.secondi = sTime.Seconds;
  tminmax.valid_max = 0;
  tminmax.valid_min = 0;

  last_ServerTime.mese =  sDate.Month;
  last_ServerTime.ora = 20;             //Aggiorna alle 20
  
  rx_state = 0;
  
  while(1){
    
    rx_state = 0;
    HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);    
    //**** Ora di aggiornare ora ?
    if(sDate.Month != last_ServerTime.mese){
      if(sTime.Hours == last_ServerTime.ora){
          //Update time server
          //**** Get Data/time dal server
          Bsp_power_ON_ESP();
          if(get_data_time_in_server(esp)==0){
            sTime.Hours = datatime.ora;
            sTime.Minutes = datatime.minuti;
            sTime.Seconds = datatime.secondi;
            sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
            sTime.StoreOperation = RTC_STOREOPERATION_RESET;
            HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            sDate.WeekDay = RTC_WEEKDAY_MONDAY;
            sDate.Month = datatime.mese;
            sDate.Date = datatime.giorno;
            sDate.Year = datatime.anno;
            HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
            last_ServerTime.ora = 20;
            last_ServerTime.mese =  sDate.Month;
            rx_state = 1;
          }else{
            //Fallita riprova piu tardi
            if(last_ServerTime.ora == 20){
              last_ServerTime.ora = 21;          
            }else{
              last_ServerTime.ora = 20;
            }
            rx_state = 1;
          }           
          rx_state = 1;
          Bsp_power_OFF_ESP();
      }    
    }
    
    //**** STOP MODE
    go_stop(rx_state);
    
    //Attivata periferiche
  //  HAL_Init();
  //  SystemClock_Config();
    MX_GPIO_Init();
    
    //**** Aspetta RX nodo
    get_radio(radio);

    //*** Get time now
    HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);    
    timenow.ora = sTime.Hours;
    timenow.minuti = sTime.Minutes;
    timenow.secondi = sTime.Seconds;
    timenow.anno = sDate.Year;
    timenow.mese = sDate.Month;
    timenow.giorno = sDate.Date;
    
    if(timenow.giorno != lasttime.giorno){
      //cambio giorno, reset min/max
      tminmax.valid_max = 0;
      tminmax.valid_min = 0;
      tminmax.TempMax = -1000.0;
      tminmax.TempMin = 1000.0;      
    
      lasttime.giorno = timenow.giorno;      
    }
    
    //**** Temp min/max
    if(recivePlayload.Temp > tminmax.TempMax){
      tminmax.TempMax = recivePlayload.Temp;

      HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);     
      tminmax.tMax.anno = sDate.Year;
      tminmax.tMax.mese = sDate.Month;
      tminmax.tMax.giorno = sDate.Date;
      tminmax.tMax.ora = sTime.Hours;
      tminmax.tMax.minuti = sTime.Minutes;
      tminmax.tMax.secondi = sTime.Seconds;
      tminmax.valid_max = 1;
    }
    if(recivePlayload.Temp < tminmax.TempMin){
      tminmax.TempMin = recivePlayload.Temp;
      
      HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);
      tminmax.tMin.anno = sDate.Year;
      tminmax.tMin.mese = sDate.Month;
      tminmax.tMin.giorno = sDate.Date;
      tminmax.tMin.ora = sTime.Hours;
      tminmax.tMin.minuti = sTime.Minutes;
      tminmax.tMin.secondi = sTime.Seconds;
      tminmax.valid_min = 1;
    }
         
    update_lcd(recivePlayload,tminmax, timenow);
    
    
    //*******************Update sul server    
    bool findOra = false;
    for(int i = 0; i < sizeof(ORAMINUTI_TXSERVER); i++){
      if(timenow.ora == ORAMINUTI_TXSERVER[i]){
        findOra = true;
        break;
      }
    }   
    uint8_t minutitx = timenow.minuti / 30;
    if((minutitx == 0)&&(index_ORAMINUTI_TXSERVER == 1)){
      index_ORAMINUTI_TXSERVER = 0;
      //tx
      //verfica ora
      if(findOra){
        Bsp_power_ON_ESP();
        if(update_temp_toServer(esp, &timenow, &recivePlayload, &tminmax)==0){
          sTime.Hours = datatime.ora;
          sTime.Minutes = datatime.minuti;
          sTime.Seconds = datatime.secondi;
          sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
          sTime.StoreOperation = RTC_STOREOPERATION_RESET;
          HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
          sDate.WeekDay = RTC_WEEKDAY_MONDAY;
          sDate.Month = datatime.mese;
          sDate.Date = datatime.giorno;
          sDate.Year = datatime.anno;
          HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);          
        }
      }
    }else if((minutitx > 0)&&(index_ORAMINUTI_TXSERVER == 0)){
      index_ORAMINUTI_TXSERVER = 1;
      //tx
      //verfica ora
      if(findOra){
        Bsp_power_ON_ESP();
        if(update_temp_toServer(esp, &timenow, &recivePlayload, &tminmax)==0){
          sTime.Hours = datatime.ora;
          sTime.Minutes = datatime.minuti;
          sTime.Seconds = datatime.secondi;
          sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
          sTime.StoreOperation = RTC_STOREOPERATION_RESET;
          HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
          sDate.WeekDay = RTC_WEEKDAY_MONDAY;
          sDate.Month = datatime.mese;
          sDate.Date = datatime.giorno;
          sDate.Year = datatime.anno;
          HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);          
        }
      }
    }
    
    
  }
    
    
    
    
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
//----------------------------------
  
/*
  mS
320.108
319.980
319.996
319.961
320.545
320.453
318.861			//batt a 3.0V
318.717
318.704
318.841
329.511
318.835
batt < 2.7 temp error  
  
*/


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

  radio.printDetails(); 
   
  radio.startListening();
  //[RX]
  
  start_temp = HAL_GetTick();
  while(1){
 
  

    while ( radio.available() ){
      stop_temp = HAL_GetTick() - start_temp;
      start_temp = HAL_GetTick();
      
      printf("Recive data\r\n");
      
      int len = radio.getDynamicPayloadSize();
      
      if(len == 12){
        radio.read( &recivePlayload, sizeof(recivePlayload));
      
        printf("[LEN]:\r\n");
        printf("%d",len);      
        
        printf("[DATA]:\r\n");
        printf("Temp: %f\r\n",recivePlayload.Temp);      
        printf("Umidita: %f\r\n",recivePlayload.Hum);      
        printf("Temp: %f\r\n",recivePlayload.Battery);      

        printf("\r\n");
        printf("\r\n");    
        
      }else{
         printf("ERRORE RX\r\n");      
      }
      radio.stopListening();
     
      radio.startListening();
      
    }

    

  }   

  
  
  
  
  
  
  
  
  
  
  
  while(1){
  
    BSP_EPD_DrawHLine(0,9,172);
    BSP_EPD_DrawVLine(86,0,8);
    BSP_EPD_SetFont(&Font12);
    BSP_EPD_DisplayStringAt(30, 5, (uint8_t*)"MIN", LEFT_MODE);
    BSP_EPD_DisplayStringAt(120, 5, (uint8_t*)"MAX", LEFT_MODE);


    BSP_EPD_SetFont(&Font12);
    BSP_EPD_DisplayStringAt(18, 0, (uint8_t*)"-10.2", LEFT_MODE);
    BSP_EPD_DisplayStringAt(115, 0, (uint8_t*)"35.4", LEFT_MODE);
    BSP_EPD_DrawRect(56,2,2,1);
    BSP_EPD_DrawRect(146,2,2,1);

    
    BSP_EPD_SetFont(&Font8);   
    BSP_EPD_DisplayStringAt(20, 3, (uint8_t*)"10:30:05", LEFT_MODE);
    BSP_EPD_DisplayStringAt(110, 3, (uint8_t*)"14:28:45", LEFT_MODE);
        

    
    BSP_EPD_SetFont(&Font8);   
    BSP_EPD_DisplayStringAt(68, 15, (uint8_t*)"17:11:38", LEFT_MODE);
    
    BSP_EPD_SetFont(&Font20);
    BSP_EPD_DisplayStringAt(15, 10, (uint8_t*)"28.7", LEFT_MODE);
    BSP_EPD_DisplayStringAt(120, 10, (uint8_t*)"30%", LEFT_MODE);
    BSP_EPD_DrawRect(72,13,2,1);
    
    BSP_EPD_RefreshDisplay();
  
    while(1){
  
  }
  
  
  
  
  
  
  
  
  
  
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  HAL_Delay(5000);
  /*
    resolution = 16/39000 = 0.000410256 s
    value a ms = 0.001 / 000410256 = 2.43750243750

    value_counter = 2.43750243750 * mS
    value_counter = 2437.50243750 * S

    value_counter < 65535  => max [26886 mS] => [26.8 S]
  */
  //Test STOP MODE --> WAKE UP rtc_wakeup ~10sec
  SystemPower_Config_OFF();
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 25000, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  
  //Attivata periferiche
  
  SystemClock_Config();
  MX_GPIO_Init();
  spi_RF24_init();
  Bsp_Init();

  while(1){
    __NOP();
    __NOP(); 
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }
  
  
  
  
  
  
  
  spi_RF24_init();
  Bsp_Init();
  
  __NOP(); 
  __NOP();
  //DEBUG
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setChannel(90);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX); 
  radio.setRetries(5,15);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]); 

  radio.printDetails(); 
  
  __NOP();
  __NOP();

  
  
  
    
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  uint8_t len = 0;
  uint8_t receive_payload[32];
   
  //[TX]
  
  while(1){
    
    static char send_payload[] = "Ciao 123456";

    radio.stopListening();
 
    radio.write( send_payload, 12);

    HAL_Delay(1000);  
  
  
  
  
  }
  
  
  //[RX]
  while(1){
  
    radio.startListening();

    while ( radio.available() ){
      
      printf("Recive data\r\n");
      
      len = radio.getDynamicPayloadSize();
      if(!len){
          printf("ERRORE LEN \r\n");
      }
      radio.read( &receive_payload[0], len );
    
      receive_payload[len] = 0;

      printf("[LEN]:\r\n");
      printf("%d",len);      
      
      printf("[DATA]:\r\n");
      printf("%s",&receive_payload[0]);      
      printf("\r\n");
      printf("\r\n");    
      
      radio.stopListening();
  
    
    }
  
  
  
  
  }  
  
  
  
  
  
  
  
    BSP_EPD_SetFont(&Font16);
  //  BSP_EPD_DrawImage(0, 0, 72, 172, Background_pic);
    BSP_EPD_DisplayStringAt(0, 13, (uint8_t*)"STM32L053", RIGHT_MODE);
    BSP_EPD_SetFont(&Font12);
    BSP_EPD_DisplayStringAt(87, 8, (uint8_t*)"DISCOVERY", LEFT_MODE);
    BSP_EPD_DisplayStringAt(71, 3, (uint8_t*)"DEMONSTRATION", LEFT_MODE);
    BSP_EPD_DisplayStringAt(94, 0, (uint8_t*)"V1.0.0", LEFT_MODE);
    BSP_EPD_RefreshDisplay();
    HAL_Delay(2000);

    
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;  
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* RTC init function */
static void MX_RTC_Init(void)
{
__HAL_RCC_RTC_ENABLE();

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  RTC_AlarmTypeDef sAlarm;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv =  85;    // 85 = 500Hz
  hrtc.Init.SynchPrediv = 500; //265; //180=355ms ; 220=423ms ; 240=482mS ; 260=488ms ; 280=562ms ;380 --499;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE; //RTC_OUTPUT_REMAP_POS1
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_RCC_RTC_ENABLE(); 
  HAL_NVIC_SetPriority(RTC_IRQn, 0x0, 0);
  HAL_NVIC_EnableIRQ(RTC_IRQn);
  
  
  /*
  GPIO_InitTypeDef  GPIO_InitStruct; 
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_RTC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  
  
  //Test
  HAL_RTCEx_SetCalibrationOutPut(&hrtc,RTC_CALIBOUTPUT_512HZ);
  //HAL_RTCEx_SetCalibrationOutPut(&hrtc,RTC_CALIBOUTPUT_1HZ);
  __HAL_RTC_CALIBRATION_OUTPUT_ENABLE(&hrtc);
  */
}




/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> TSC_G1_IO3
     PA3   ------> TSC_G1_IO4
     PA6   ------> TSC_G2_IO3
     PA7   ------> TSC_G2_IO4
     PB0   ------> TSC_G3_IO2
     PB1   ------> TSC_G3_IO3
     PA9   ------> USART1_TX
     PA10   ------> USART1_RX
     PA15   ------> SPI1_NSS
     PB3   ------> SPI1_SCK
     PB5   ------> SPI1_MOSI
     PB8   ------> I2C1_SCL
     PB9   ------> I2C1_SDA
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : MFX_IRQ_OUT_Pin */
  GPIO_InitStruct.Pin = MFX_IRQ_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MFX_IRQ_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MFX_WAKEUP_Pin ePD1_BUSY_Pin */
  GPIO_InitStruct.Pin = MFX_WAKEUP_Pin|ePD1_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD_R_Pin */
  GPIO_InitStruct.Pin = LD_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF3_TSC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ePD1_RESET_Pin ePD1_PWR_ENn_Pin ePD1_D_C_Pin LD_G_Pin */
  GPIO_InitStruct.Pin = ePD1_RESET_Pin|ePD1_PWR_ENn_Pin|ePD1_D_C_Pin|LD_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ePD1_CS_Pin */
  GPIO_InitStruct.Pin = ePD1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(ePD1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ePD1_SCK_Pin ePD1_MOSI_Pin */
  GPIO_InitStruct.Pin = ePD1_SCK_Pin|ePD1_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MFX_I2C_SCL_Pin MFX_I2C_SDA_Pin */
  GPIO_InitStruct.Pin = MFX_I2C_SCL_Pin|MFX_I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ePD1_RESET_Pin|ePD1_PWR_ENn_Pin|ePD1_D_C_Pin|LD_G_Pin, GPIO_PIN_RESET);

}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  /*
  while(1) 
  {
  }
  */
  /* USER CODE END Error_Handler */ 
}

void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *  huart ){

  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Clear Wake Up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
