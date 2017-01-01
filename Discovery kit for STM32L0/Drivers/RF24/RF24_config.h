
/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
 /* spaniakos <spaniakos@gmail.com>
  Added __ARDUINO_X86__ support
*/

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__

#include "stm32l0xx_hal.h"
  /*** USER DEFINES:  ***/  
  //#define FAILURE_HANDLING
  //#define SERIAL_DEBUG
  //#define MINIMAL
  //#define SPI_UART  // Requires library from https://github.com/TMRh20/Sketches/tree/master/SPI_UART
  //#define SOFTSPI   // Requires library from https://github.com/greiman/DigitalIO
  
  /**********************/
  #define rf24_max(a,b) (a>b?a:b)
  #define rf24_min(a,b) (a<b?a:b)


 
  // RF modules support 10 Mhz SPI bus speed
  const uint32_t RF24_SPI_SPEED = 10000000;  





#endif // __RF24_CONFIG_H__

