

#include "ioCC1110.h"
#include "rf_cc1110.h"
extern void DMA_init(void);
volatile INT8U len;
    int main( void )
{
    INT8U buffer[10], rssi, lqi;
  
    P1DIR |= 0x03;                //P1_0, p1_1, OUTPUT
  
    CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
    

    rf_cc1110_init( 433000 );
	DMA_init();
	DMAARM=1;		//DMA0 ARM 	
    IEN0 = 0x81;
    
    while( 1 ){
      
   //  len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
     if( len == 10 )
     {
       //  P1 ^= 0x03    ;        //�յ����ݣ�LED��תһ��
      //   rf_send_packet( buffer, 10 );//����Ӧ���ź�
     }
     len = 0;
    }
  
  
  

}