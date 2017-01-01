#include "esp8266_ll.h"



volatile uint8_t p_r;
volatile uint8_t p_w;

volatile uint8_t ESP_BUFFER[BUFFER_RX_ESP8266];

volatile uint8_t p_wt;
  
UART_HandleTypeDef UartHandle;
  
/******************************************************************************/
/******************************************************************************/
/***   Copy this file to project directory and rename it to "esp8266_ll.c"   **/
/******************************************************************************/
/******************************************************************************/
void ESP_LL_Init_RESET(void) {
   GPIO_InitTypeDef  GPIO_InitStruct;
  //RESET SIGNAL
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = (GPIO_PIN_4);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH  ; 
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  GPIOA->BSRR = GPIO_PIN_4;  
}  
uint8_t ESP_LL_Init(void) {
    /* Init UART with desired baudrated passed in LL structure */

    /* Init reset pin and set it high */
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitStruct.Pin       = GPIO_PIN_9;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
 
  
  
  UartHandle.Instance        = USART1;
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT | UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  UartHandle.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;

  HAL_UART_Init(&UartHandle);
 /* NVIC for USART1 */
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);  
  
  p_r = 0;
  p_wt = 0;
  HAL_UART_Receive_IT(&UartHandle, (uint8_t * )&ESP_BUFFER[p_wt], 1);

  p_wt = 1;
  
  p_w = 0;
  return 0;
}


uint8_t ESP_LL_SendData(const uint8_t* data, uint16_t count) {
    /* Send data via UART */
    HAL_UART_Transmit(&UartHandle,(uint8_t*)data,count,HAL_MAX_DELAY); 

    /* Everything OK, data sent */
    return 0;
}


uint8_t ESP_LL_SetReset(uint8_t state) {
    /* Set reset pin */
    if (state == 1) {
        /* Set pin low */
      /*Reset PB4*/
      GPIOA->BRR = GPIO_PIN_4;        
    } else {
        /* Set pin high */
     /*Set PB4*/
      GPIOA->BSRR = GPIO_PIN_4;      
    }

    /* Everything OK, RST pin set according to state */
    return 0;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandlet)
{
  if (UartHandlet->Instance == USART1)  //current UART
  {
    HAL_UART_Receive_IT(&UartHandle, (uint8_t * )&ESP_BUFFER[p_wt++], 1);
    p_w++;
  }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
volatile uint32_t errorcode;

 void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  errorcode = UartHandle->ErrorCode;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();    

}


