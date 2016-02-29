

#include "ioCC1110.h"

#include "rf_cc1110.h"

#include "hal_cc1110.h"


#pragma vector = RF_VECTOR
 __interrupt void RF_ISR1(void)
 {
    S1CON &= ~0x03;
 }
 
 
 #pragma vector = ST_VECTOR
 __interrupt void SLEEP_TIMER_ISR(void)
 {
    IRCON &= ~( 1<<7 ); 
    WORIRQ &= ~( 1<<0 );
 }

int main( void )
{
    INT8U  test[10] = {1,2,3,4,5,6,7,8,9,10}, rssi, lqi, len;
    INT8U  buffer[10];
    volatile INT32U  delay;
  
    P1DIR |= ( 1<<0 ) | ( 1<<1 );// 设置P1_0, P1_1为输出   
  
    CLKCON &= ~( 1<<7 );          //32.768
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
    
    RFIM = 0xFF;
    rf_cc1110_init( 433000 );
    
      
    WORCTL |= 0x00;    //2^15 period
    IEN0 |= 0X20;     //开中断
    WORIRQ |= 0X10;   //

    EA = 1;
    
  
  //循环发射，接收方收到后，LED会翻转
  //此处采用sleep timer，它在实际项目中非常有用，时钟源为外部32768高精度晶振
  while( 1 )
  {
    SLEEP &= ~0x07;   //PM0 mode
    SLEEP |= 0x01;    //PM1 mode
    WORCTL |= 0x04;
    PCON |= 0x01;     //进入PM1模式
  
    while(!(SLEEP & 0x40));      //等待晶振稳定
    SIDLE();
    SCAL();
    while(MARCSTATE != 0x01);
    for( delay = 0; delay < 20000; delay ++ );
    
    P1 &= ~( 1<<0 );
    for( delay = 0; delay < 20000; delay ++ );
    rf_send_packet( test, 10 );     //
    P1 |= ( 1<<0 );
    //每发射一次，闪烁一次。
      
    for( delay = 0; delay < 2; delay ++ )
    {
      WORCTL |= 0x04;
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
      if( len == 10 )   { break; } 
    }
    if( delay >= 2 ) { P1 |= ( 1<<1 ); } //如果超时，说明没收到应答，灭灯
    else             { P1 &= ~( 1<<1 ); }//收到应答，亮灯
    
  }    
}