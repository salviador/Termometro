#ifndef __SPIRF24_H__
#define __SPIRF24_H__

#include "stm32l0xx_hal.h"
#include <stdbool.h> 

#define LOW false
#define HIGH true

#ifdef __cplusplus
extern "C" {
#endif

void spi_RF24_init(void);
void spi_RF24_deinit(void);

void spi_RF24_tx(uint8_t* aTxBuffer, uint8_t * aRxBuffer, uint8_t len);

void gpio_RF24_init(void);
void ce_RF24(bool v);
void csn_RF24(bool v);


#ifdef __cplusplus
}
#endif








#endif // __RF24_H__

