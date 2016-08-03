/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor. The use, copying, transfer or disclosure of such information 
 * is prohibited except by express written agreement with Nordic Semiconductor.
 *
 * $LastChangedRevision:$
 */

/** @file
 *
 * The nRF24LE1 example application.
 *
 * @author Rune Brandsegg
 * @author Ivar C. OEsthus
 *
 */

#include "hal_nrf.h"
#include "system.h"
#include "radio.h"
#include "application.h"

#include <stdint.h>


#define LED_DELAY	100
volatile uint8_t	gnUartData;
uint8_t xdata packet[RADIO_MAX_PL];     // Buffer for sending or receiving
                                        // packet.
void transmitter_mode(void)
{
  LED2 = OFF;                           // LED2 blank, indicates PTX mode
  while(true)
  {
    while(radio_busy())                 // Wait until radio ready
      ;

    if(!B0)
    {
      send_packet(CMD1);
      while(!B0)
        ;                               // Wait until B0 relesed...
    }
    if(!B1)
    {
      send_packet(CMD2);
      while(!B1)
        ;                               // Wait until B1 relesed...
    }
  }
}
void receiver_mode(void)
{
  uint8_t command;
  uint8_t time_delay=0;
  LED2 = ON;                            // LED2 lit, indicates PRX mode
 LED0_OFF;
  while(true)
  {
    if(radio_data_received())
    {
      command = read_packet();
      switch(command)
      {
        case CMD1:                      // "Link-packet" received
         // LED0_BLINK();
          //LED0=!LED0;
		  time_delay=0;
		  LED0_ON;
		  break;

        case CMD2:                      // "Boot message packet" received
          boot_mess();                  // Show boot message
          LED2 = ON;
          break;

        default:
          break;
      }
    }
//	LED0_ON;
//	delay_10ms();
	delay_10ms();
	delay_10ms();
//	LED0=!LED0;
	if(time_delay++>LED_DELAY)
	{
		LED0_OFF;

  		time_delay=0;
	}
  }
}

void send_packet(uint8_t command)
{
  bool packet_sent;
  uint8_t data_length;

  LED0 = ON;                            // Trans.; flash LED1...

  /* Assemble packet  */
  packet[0] = CMD_SEND_DATA;            // Add TYPE (data = 0x00)
  packet[1] = data_length = 1;          // Add LENGTH (1 byte)
  packet[DATA_POS] = command;           // Add DATA to send

  /* 
     .... Room for encryption of message .... 
                                              */

  /* Send packet with radio */
  radio_send_packet(packet, data_length+DATA_POS);

  /* Wait for respons from radio, TX_DS or MAX_RT */
  while(radio_busy())
    ;

  packet_sent = radio_packet_sent();

  if(packet_sent)
  {
    LED1_BLINK();                       // Packet sent, flash LED1
  }
  else
  {
    LED2_BLINK();                       // Packet lost, flash LED2
  }
  LED0 = OFF;
}

uint8_t read_packet(void)
{
  uint8_t command, data_length,type, pck_length;

  /* Get the packet	from radio */
  pck_length = radio_read_data(packet);

  /* Disassemble packet */
  type        =  packet[0];             // Get TYPE of message
  data_length =  packet[1];             // Get LENGTH of data part

  /* .... Room for decryption of message .... */

  command = packet[DATA_POS];           // Get the received command
  return command;
}

void radio_irq(void) interrupt RF_READY_INT_VECT  
{
  radio_interrupt();  
}



//---------------------------------------------------------------------------
void ri0_interrupt(void) interrupt INTERRUPT_UART	 
{ 	
	if (RI0 == 1)
  	{
    	RI0 = 0;	 
		gnUartData = S0BUF; 
		S0BUF = gnUartData;								
  	}
	else if(TI0 == 1)
	{
		TI0 = 0; 			
	}
}