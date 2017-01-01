/**
  ******************************************************************************
  * @file    bsp.c 
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    31-May-2016
  * @brief   Main program body
  *          This example code shows how to use the Nucleo BSP Drivers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "bsp.h"

void Bsp_Init(void)
{
  /* Configure LED3 and LED4 */
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  
  /* Initialize the User button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
  
  /* Initialize the EPD */
  BSP_EPD_Init();
  

}


void Bsp_power_ON_radio(void){
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  __HAL_RCC_GPIOH_CLK_ENABLE();
  GPIO_InitStruct.Pin = (GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW ; 
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  //LOW --> Attiva swtitch ON
  GPIOH->BRR = GPIO_PIN_1;   
}


void Bsp_power_ON_ESP(void){
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = (GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW ; 
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
  //LOW --> Attiva swtitch ON
  GPIOB->BRR = GPIO_PIN_12;   
  HAL_Delay(1000);
}

void Bsp_power_OFF_ESP(void){
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = (GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW ; 
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

}



