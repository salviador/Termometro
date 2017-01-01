#include "spiRF24.h"


SPI_HandleTypeDef hspi2;

static GPIO_InitTypeDef  GPIO_CE_InitStruct;
static GPIO_InitTypeDef  GPIO_CSN_InitStruct;
static GPIO_InitTypeDef  GPIO_InitStruct;

void spi_RF24_deinit(void){
 __HAL_RCC_SPI2_CLK_ENABLE();
   HAL_SPI_DeInit(&hspi2);
 __HAL_RCC_SPI2_CLK_DISABLE();
 
}

void spi_RF24_init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();
  
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  HAL_NVIC_SetPriority(SPI2_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(SPI2_IRQn);
  
  
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspi2);
  
  /*
  
 hspi2.Instance               = SPI2;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.Direction         = SPI_DIRECTION_2LINES;
  hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi2.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial     = 7;
  hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi2.Init.NSS               = SPI_NSS_SOFT;
  hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi2.Init.Mode = SPI_MODE_MASTER;

  HAL_SPI_Init(&hspi2);
  */
}

void spi_RF24_tx(uint8_t* aTxBuffer, uint8_t * aRxBuffer, uint8_t len){
  
  HAL_SPI_TransmitReceive_IT(&hspi2, aTxBuffer, aRxBuffer, len);
  while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY)
  {
  } 
}


void gpio_RF24_init(void){

  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_CE_InitStruct.Pin = (GPIO_PIN_7);
  GPIO_CE_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_CE_InitStruct.Pull = GPIO_PULLUP;
  GPIO_CE_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH  ; 
  HAL_GPIO_Init(GPIOB, &GPIO_CE_InitStruct); 
  ce_RF24(LOW);
    
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_CSN_InitStruct.Pin = (GPIO_PIN_6);
  GPIO_CSN_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_CSN_InitStruct.Pull = GPIO_PULLUP;
  GPIO_CSN_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH  ; 
  HAL_GPIO_Init(GPIOB, &GPIO_CSN_InitStruct); 
  csn_RF24(HIGH);

}
  

void ce_RF24(bool v){
  if(v)
    /*Set PB4*/
    GPIOB->BSRR = GPIO_PIN_7;
  else
    /*Reset PB4*/
    GPIOB->BRR = GPIO_PIN_7;    
}

void csn_RF24(bool v){
  if(v)
    /*Set PB4*/
    GPIOB->BSRR = GPIO_PIN_6;
  else
    /*Reset PB4*/
    GPIOB->BRR = GPIO_PIN_6;    
}



  