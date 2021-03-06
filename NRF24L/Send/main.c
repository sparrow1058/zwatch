/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor. The use, copying, transfer or disclosure of such information 
 * is prohibited except by express written agreement with Nordic Semiconductor.
 *
 * $LastChangedRevision:$
 */

/** @file
 * The nRF24LE1 example application main file
 *
 * @author Rune Brandsegg

 */
 
#include "reg24le1.h"
#include "hal_nrf.h"
#include "radio.h"
#include "application.h"
#include "system.h"





void main(void) 
{ 
  mcu_init();
                                          
  
  EA = 1;                               // Global interrupt enable
  boot_mess();

  radio_init(TRANSMITTER);
  transmitter_mode();

  /*
  if(!B0)                               // Pressed B0 during "boot":
  {                                     // Enter PRX mode (Receiver)        
    radio_init(RECEIVER);
    receiver_mode();                  
  }  
  else                                  // Released B0 during "boot":
  {                                     // Enter PTX mode (Transmitter)
    radio_init(TRANSMITTER);
    transmitter_mode();
  }
  */
}
