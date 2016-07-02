/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor. The use, copying, transfer or disclosure of such information 
 * is prohibited except by express written agreement with Nordic Semiconductor.
 *
 * $LastChangedRevision:$
 */
 
/** @file
 * Radio header file for the nRF24LE1 example application
 * 
 * @author Rune Brandsegg
 */

#define RF_IDLE 0x00
#define RF_BUSY 0xff
#define FIFO_EMPTY 0x01

/** This function initializes the radio interface,
 *  and makes a complete radio setup.
 */
void radio_init(uint8_t operation_mode);

/** Function to enable SPI interface to the radio 
 */
void nrf_spi_init(void);

/** This function load the bytes to be sent into the radio, 
 *  starts transmission, and returns
 */
void radio_send_packet(uint8_t *payload, uint8_t length);

/** This function returns true if the radio is busy
 */
bool radio_busy(void);

/** This function returns true if the packet was successfully sent, and false 
 *  if the transmission failed after maximum number of retries.
 */
bool radio_packet_sent(void);

/** This function returns true if the radio have received new data
 */
bool radio_data_received(void);

/** This function saves the data from the radio to the specified array.
 *  @return the width of the received data.
 */
uint8_t radio_read_data(uint8_t *rx_data);

void radio_interrupt(void);