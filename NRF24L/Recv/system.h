/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor. The use, copying, transfer or disclosure of such information 
 * is prohibited except by express written agreement with Nordic Semiconductor.
 *
 * $LastChangedRevision:$
 */
 
/** @file
 * System header file for the nRF24LE1 example application
 * 
 * @author Rune Brandsegg
 */
#ifndef SYSTEM_H__
#define SYSTEM_H__

#include "nordic_common.h"

#define B0 P04
#define B1 P05
#define B2 P06

#define LED0 P03
#define LED1 P01
#define LED2 P02
#define LED0_ON		LED0=1
#define LED0_OFF	LED0=0



#define ON 1
#define OFF 0

#define TRANSMITTER 0
#define RECEIVER 1

/** low level macros */
#define LED0_BLINK() do{  LED0 = ON;        \
                          delay_10ms();     \
                          LED0 = OFF;       \
                          }while(0)

#define LED1_BLINK() do{  LED1 = ON;        \
                          delay_10ms();     \
                          LED1 = OFF;       \
                          }while(0)

#define LED2_BLINK() do{  LED2 = ON;        \
                          delay_10ms();     \
                          LED2 = OFF;       \
                          }while(0)

/** Function used to initialize the system, ports etc.
 */
void mcu_init(void);

/** This function flashes the leds in a certain pattern to indicate system boot
 */
void boot_mess(void);

/** Function that gives 10ms delay.
 */
void delay_10ms(void);

#endif // SYSTEM_H__
