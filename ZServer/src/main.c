

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
  
    P1DIR |= ( 1<<0 ) | ( 1<<1 );// ����P1_0, P1_1Ϊ���   
  
    CLKCON &= ~( 1<<7 );          //32.768
    CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
    
    RFIM = 0xFF;
    rf_cc1110_init( 433000 );
    
      
    WORCTL |= 0x00;    //2^15 period
    IEN0 |= 0X20;     //���ж�
    WORIRQ |= 0X10;   //

    EA = 1;
    
  
  //ѭ�����䣬���շ��յ���LED�ᷭת
  //�˴�����sleep timer������ʵ����Ŀ�зǳ����ã�ʱ��ԴΪ�ⲿ32768�߾��Ⱦ���
  while( 1 )
  {
    SLEEP &= ~0x07;   //PM0 mode
    SLEEP |= 0x01;    //PM1 mode
    WORCTL |= 0x04;
    PCON |= 0x01;     //����PM1ģʽ
  
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    SIDLE();
    SCAL();
    while(MARCSTATE != 0x01);
    for( delay = 0; delay < 20000; delay ++ );
    
    P1 &= ~( 1<<0 );
    for( delay = 0; delay < 20000; delay ++ );
    rf_send_packet( test, 10 );     //
    P1 |= ( 1<<0 );
    //ÿ����һ�Σ���˸һ�Ρ�
      
    for( delay = 0; delay < 2; delay ++ )
    {
      WORCTL |= 0x04;
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
      if( len == 10 )   { break; } 
    }
    if( delay >= 2 ) { P1 |= ( 1<<1 ); } //�����ʱ��˵��û�յ�Ӧ�����
    else             { P1 &= ~( 1<<1 ); }//�յ�Ӧ������
    
  }    
}