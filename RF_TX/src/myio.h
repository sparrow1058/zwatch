#ifndef __IOINIT__H
#define	__IOINIT__H

#define LED1  P1_0
#define LED2  P1_1
#define KEY1  P1_3
#define KEY2  P1_2
#define KEY_NO_KEY  0
#define KEY_DOWN    1;
#define KEY_SHORT_PRESS 2
#define KEY_LONG_PRESS  3
#define TDMA_TX     	0
#define	TDMA_RX      	1

#define TESTKEY ( KEY1 ? (LED1=1 ) : (LED1=0 ) )
#define LED_ON LED1=0
#define LED_OFF  LED1=1
#endif