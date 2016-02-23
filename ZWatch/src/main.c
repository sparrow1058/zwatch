

#include "ioCC1110.h"
#include "rf_cc1110.h"
#include "tm1650.h"
#define LED1	P1_0
#define LED2	P1_1
#define KEY1    P1_3
#define KEY2    P1_2
#define TM_LED1		0x68
#define TM_LED2		0x6A
#define MYADDR		0x01

uint  RecCount=0;
void ioInit()
{
	P1DIR=0x3B;
	P0SEL!=0x30;
	P0DIR|=0x30;	//set p0_2 P0_3 out put
	LED1=1;
        LED2=1;
}
void boardInit()
{
   //   P1DIR |= 0x03;                //P1_0, p1_1, OUTPUT
      CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
   ioInit(); 
}

volatile INT8U len;
    int main( void )
{
    INT8U buffer[10], rssi, lqi;
    boardInit();
    rf_cc1110_init( 433000 );
    IEN0 = 0x81;
    
    while( 1 ){
      
     len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
     if( len == 10 )
     {
       
       P1 ^= 0x03    ;        //�յ����ݣ�LED��תһ��
      rf_send_packet( buffer, 10 );//����Ӧ���ź�
      TM1650_Show(RecCount++);
     }
     len = 0;
    }
  
  
  

}