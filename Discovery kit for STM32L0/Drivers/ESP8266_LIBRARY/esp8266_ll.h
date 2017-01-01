/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website https://majerle.eu/projects/esp8266-at-commands-parser-for-embedded-systems
 * \license MIT
 * \brief   Low level, platform dependant, part for communicate with ESP module and platform.
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef ESP_LL_H
#define ESP_LL_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"

#define BUFFER_RX_ESP8266 255
  
//buffer RX


  
  
  
  
void ESP_LL_Init_RESET(void);
uint8_t ESP_LL_Init(void);
uint8_t ESP_LL_SendData(const uint8_t* data, uint16_t count);
uint8_t ESP_LL_SetReset(uint8_t state);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
