#include "mytypedef.h"
#include "myio.h"
#include <iocc1110.h>
#include <string.h>


extern void UartSendString(uchar *data,int len);
extern void SingleMoreTDMA(char sell);
extern void InitUART(void);
uchar uartChar;
uchar keyDown,keyCount;
void ioInit()
{
	P1DIR=0x3B;
	P0SEL!=0x30;
	P0DIR|=0x30;	//set p0_2 P0_3 out put
	LED1=0;
    LED2=1;
    
    //  P1DIR|=0x03;
 // P1DIR&=0xF3;  //KEY1 =p1_3  KEY2 =P1_2
  //LED1=1;
  //LED2=1;
}
INT8U getKeyCode()
{
	INT8U ret=KEY_NO_KEY;	//default key no key
	if(KEY1==0)
	{
		keyDown=1;
		keyCount++;
		ret=KEY_DOWN;		//key down
	}else
	{
		if(keyDown==1)
		{
			if(keyCount<100)
				ret= KEY_SHORT_PRESS;	//short press
			else
				ret= KEY_LONG_PRESS;	//Long press
			keyDown=0;
			keyCount=0;
		
		}
	}
	return ret;
	


}

#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{
	URX0IF=0;
	uartChar=U0DBUF;
}

static void  delay(unsigned long i)
{
  unsigned long j;
  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);
}

int main( void )
{
  InitUART();
  ioInit();
UartSendString("test" ,4);
   // delay(10000);
   
  while(1)
  {

  #ifdef CC1110_TDMA_RX
	SingleMoreTDMA(TDMA_RX);
#endif
	#ifdef CC1110_TDMA_TX
	SingleMoreTDMA(TDMA_TX);
	#endif 
             
  }
        
}