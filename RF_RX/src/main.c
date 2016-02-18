

#include "ioCC1110.h"
#include "rf_cc1110.h"
extern void DMA_init(void);
volatile INT8U len;
    int main( void )
{
    INT8U buffer[10], rssi, lqi;
  
    P1DIR |= 0x03;                //P1_0, p1_1, OUTPUT
  
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
    

    rf_cc1110_init( 433000 );
	DMA_init();
	DMAARM=1;		//DMA0 ARM 	
    IEN0 = 0x81;
    
    while( 1 ){
      
   //  len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
     if( len == 10 )
     {
       //  P1 ^= 0x03    ;        //收到数据，LED翻转一次
      //   rf_send_packet( buffer, 10 );//发送应答信号
     }
     len = 0;
    }
  
  
  

}