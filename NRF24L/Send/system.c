/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor. The use, copying, transfer or disclosure of such information 
 * is prohibited except by express written agreement with Nordic Semiconductor.
 *
 * $LastChangedRevision:$
 */

/** @file
 * System level functions.
 * 
 * @author Rune Brandsegg
 *
 */
#include <stdint.h>
#include "hal_nrf.h"
#include "radio.h"
#include "system.h"

void mcu_init(void)
{  
	EA = 0;

	// CLK
	CLKCTRL = 0x28;			// Clock sourced by XCOSC16M  	
	CLKLFCTRL = 0x01; 		// RCOSC32K 

	delay_10ms();
	delay_10ms();
	delay_10ms();
	delay_10ms();
	delay_10ms();
	delay_10ms();
	

	// RTC		
	OPMCON = 0x00; 

   	RTC2CMP0 = 0x00;
	RTC2CMP1 = 0x80; 
								  
	RTC2CON = 0x07;						

	WUOPC1 = 0x00;
	WUOPC0 = 0x00;

	WUCON = 0xEB;	 	

  	// IO
	ADCCON1 = 0x00;
	ADCCON2 = 0x00;
	ADCCON3 = 0x00;
	COMPCON = 0x00; 
	PWMCON = 0x00;

	P0 = 0; 
	P1 = 0;
	P0DIR = 0x00;
	P1DIR = 0x00; 	

	// UART	
	P0DIR &= 0xF7;				// P03 (TxD) is output
  	P0DIR |= 0x10;     			// P04 (RxD) is input 
	P0|=0x18;	 
		
	S0CON = 0x50;  
   	PCON |= 0x80; 				// SMOD = 1
  	WDCON |= 0x80;   			// Select internal baud rate generator 
	
	//S0RELL = 0xFB; 	
  	S0RELL = 0xF3; 				// BAUD_38K4
  	S0RELH = 0x03;	
	//P0CON = 0x13;
	//P0CON = 0x44;	 	  	 
  	ES0 = 1;          			// Enable UART0 interrupt

  	RFCKEN = 1;                 // enable L01 clock
  	RF = 1;                     // enable RF interrupt  

	EA = 1;
}
void boot_mess(void)
{   
  uint8_t ctr;

  for(ctr=0;ctr<3;ctr++)
  {
    LED0_BLINK();
    delay_10ms();
    
    LED1_BLINK();
    delay_10ms();
    
    LED2_BLINK();
    delay_10ms();
  }       
}

void delay_10ms(void)
{
  uint16_t ctr;
  for(ctr=0; ctr<15000; ctr++);
}