

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
   P1DIR |= ( 1<<0 ) | ( 1<<1 );// 设置P1_0, P1_1为输出   
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
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
    
  
  //循环发射，接收方收到后，LED会翻转
  //此处采用sleep timer，它在实际项目中非常有用，时钟源为外部32768高精度晶振
  while( 1 )
  {

    while(MARCSTATE != 0x01);
     LED1=0;
    rfBuffer.macID=MSG_BROADCAST;
    rf_send_packet((INT8U *) &rfBuffer, 7 );     //
    LED1=1;
    //每发射一次，闪烁一次。
    
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
