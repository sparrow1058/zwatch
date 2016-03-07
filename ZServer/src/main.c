

#include "ioCC1110.h"

#include "rf_cc1110.h"

#include "hal_cc1110.h"

#define BUFFER_SIZE   7
#define MAX_NUM     2
#define RETRY_TIMES   20    
// msg type 
#define BROADCAST   0xFF

#define MSG_TIME      0x01
#define MSG_SUCCESS      0x02
#define MSG_BROADCAST 0xFF
#define MSG_OK		0x08

//machine ID
#define LED1    P1_0
#define LED2    P1_1
//External functions
extern void InitUART(void);
extern void UartSendString(uchar *data,int len);

typedef struct RF_DATA
{
  INT16U  num;
  INT8U   msgType;
  INT8U   macID;
  INT8U   tableID;
  INT8U   orderID;
  INT8U   sum;
}RFDATA;

RFDATA rfBuffer;
void boardInit()
{
   P1DIR |= ( 1<<0 ) | ( 1<<1 );// ����P1_0, P1_1Ϊ���   
    CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
      rf_cc1110_init( 433000 );
   // ioInit();
    IEN0 = 0x81;
}
void getRfBuffer(INT8U  *buf)
{
  INT8U *data=buf;
  rfBuffer.num=*data<<8+*(data+1);
  rfBuffer.msgType=*(data+2);
  rfBuffer.macID=*(data+3);
  rfBuffer.tableID=*(data+4);
  rfBuffer.orderID=*(data+5);
  rfBuffer.sum=*(data+6);  
}
int main( void )
{
    INT8U  test[10] = {0x01,0x2,3,0xFF,0x08,6,7,8,9,10}, rssi, lqi, len;
    INT8U  buffer[10];
    volatile INT32U  delay;
    INT16U  i;
     boardInit();
     InitUART();
     UartSendString("Zserver",7);
    rfBuffer.num=0x0102;
    rfBuffer.msgType=0x01;

    rfBuffer.tableID=0x08;
    rfBuffer.orderID=0x02;
    rfBuffer.sum=0x38;
    
  
  //ѭ�����䣬���շ��յ���LED�ᷭת
  //�˴�����sleep timer������ʵ����Ŀ�зǳ����ã�ʱ��ԴΪ�ⲿ32768�߾��Ⱦ���
  while( 1 )
  {

    while(MARCSTATE != 0x01);
     LED1=0;
    rfBuffer.macID=MSG_BROADCAST;
    rf_send_packet((INT8U *) &rfBuffer, 7 );     //
    LED1=1;
    //ÿ����һ�Σ���˸һ�Ρ�
    
    rfBuffer.msgType=MSG_TIME;
    for(i=0;i<MAX_NUM;i++)
    { 
      rfBuffer.macID=i;
      rf_send_packet((INT8U *) &rfBuffer, 7 );
      for( delay = 0; delay < RETRY_TIMES; delay ++ )
    {

      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
      if( len!=0)   
      { 
        UartSendString((uchar *)buffer,7);
        LED2=0;
        getRfBuffer(buffer);
        if(rfBuffer.msgType==MSG_OK)
        {
          rfBuffer.msgType=MSG_SUCCESS;
          rf_send_packet((INT8U *) &rfBuffer, 7 );
        }
        LED2=1;
          break; 
      }
    }
    }

  }    
}
#pragma vector = RF_VECTOR
 __interrupt void RF_ISR1(void)
 {
    S1CON &= ~0x03;
 }
