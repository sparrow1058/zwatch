#include "ioCC1110.h"
#include "rf_cc1110.h"
#include "hal_cc1110.h"
#define RF_FREQ_433MHZ  433000
#define RF_FREQ_868MHZ  868000
#define RF_FREQ_902MHZ  902000


#define BUFFER_SIZE   8
#define MAX_NUM     5
#define RETRY_TIMES   2
#define WAIT_TIMES    1
#define MSG_TIMEOUT     10
// msg type 
#define BROADCAST   0xFF

#define MSG_TIME      0x01
#define MSG_SUCCESS      0x02
#define MSG_BROADCAST   0xFF
#define MSG_OK		0x08



#define UART_MSG_ORDER    1
#define UART_MSG_ACCESS   2
#define UART_MSG_SUCCESS  3
#define UART_MSG_FAIL     4
#define UART_MSG_FREQ     0xFE    //change FREQ

//machine ID
#define LED_TX    P1_0
#define LED_RX    P1_1

#define KEY1    P1_2
#define KEY2    P1_3
//External functions
/*** Define Data iD */
typedef struct RF_MASK
{
  INT8U  max;
  INT8U  id[5];
}RFMASK;
typedef struct RF_DATA
{
  INT16U  num;
  INT8U   msgType;
  INT8U   groupID;
  INT8U   macID;
  INT8U   tableID;
  INT8U   orderID;
  INT8U   sum;
}RFDATA;
typedef struct UART_CMD
{
  INT16U  num;
  INT8U   msgType;
  INT8U   groupID;
  INT8U   macID;  
  INT8U   tableID;
  INT8U   orderID;
  INT8U   sum;
  RFMASK   idList;
 
}UARTCMD;               //19 byte
extern void InitUART(void);
extern void UartSendString(uchar *data,int len);
void handleMessage(void);
INT8U getUartCmd(void);
RFDATA rfBuffer;
UARTCMD uartCmd;
uchar uartData;
uchar uartCount=0;
uchar uartFlag=0;
uchar cmdFlag=0;
INT8U maxTimes=10;

uchar uartGet[20];
uchar sendMac[40];
uchar sendNum;
INT16U  msgTimeCount=0;
void delay_nms(unsigned int n)
{
	unsigned int i;
	while(n--)
	   for(i=0;i<550;i++);
}
void InitWatchdog(void)
{
  WDCTL=0x00;   //set watch dog time 1s
  WDCTL|=0x08;  //start watchdog
}
void feetDog(void)
{
  WDCTL=0xA8;
  WDCTL=0x58;
}
void boardInit()
{
   P1DIR |= ( 1<<0 ) | ( 1<<1 );// 设置P1_0, P1_1为输出  
   P1DIR&=0xF3;
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
      rf_cc1110_init( RF_FREQ_433MHZ );
   // ioInit();
    IEN0 = 0x81;
        WORCTL |= 0x00;    //2^15 period
    WORIRQ |= 0X10;   //
 //   IEN0 |= 0X20;     //开中断
    InitWatchdog();             //leaf init watch dog  
}
void getRfBuffer(uchar  *buf)
{
  INT8U *data=buf;
 // rfBuffer.num=*data;
  rfBuffer.num=*(data) +*(data+1)*256;
  data+=2;
  rfBuffer.msgType=*(data++);
  rfBuffer.groupID=*(data++);
  rfBuffer.macID=*(data++);
  rfBuffer.tableID=*(data++);
  rfBuffer.orderID=*(data++);
  rfBuffer.sum=*(data++);  
}
int main( void )
{
  int i;   
  boardInit();
     InitUART();
   //  UartSendString("Zserver",7);

     rfBuffer.num=0x0102;
    rfBuffer.msgType=0x01;
    rfBuffer.groupID=0xFF;
    rfBuffer.tableID=0x01;
    rfBuffer.orderID=0x02;
    rfBuffer.sum=0x38;
   
  
  //循环发射，接收方收到后，LED会翻转
  //此处采用sleep timer，它在实际项目中非常有用，时钟源为外部32768高精度晶振
  while( 1 )
  {
    
    cmdFlag=getUartCmd();
   feetDog();
#ifdef  RF_TEST  
     if(i++>200)
     {  
     if(rfBuffer.tableID++>99)
       rfBuffer.tableID=0x01;
     rfBuffer.orderID=rfBuffer.tableID%8+1;
       cmdFlag=TRUE;
     sendMac[0]=1;
     sendMac[1]=2;
     sendMac[2]=3;
     sendMac[3]=4;
     sendNum=4;
     }
#endif
      while(cmdFlag==TRUE) 
     {
      i=0;
      rf_cc1110_init( RF_FREQ_433MHZ ); //Reinit 433 register
       msgTimeCount++;
       handleMessage();
      if(msgTimeCount>maxTimes)
      {
        msgTimeCount=0;
        cmdFlag=FALSE;
        uartCmd.msgType=UART_MSG_FAIL;
        UartSendString((uchar *)&uartCmd,BUFFER_SIZE);
        
      }
      uartFlag=0;
     // uartData=0;
    //  uartCount=0;

     }
 //    SET_POWER_MODE(3);
  }    
}
 
void getMacId(RFMASK *id)
{
    uchar i,j,cnt=0;
    for(i=0;i<5;i++)
    {
      for(j=0;j<8;j++)
      {
        if((id->id[i]>>j)&0x01)
          sendMac[cnt++]=i*8+j+1;
      }
    }
    sendNum=cnt;

  
}

void handleMessage(void)
{
      INT8U rssi, lqi, len;
    INT8U  buffer[10];
    volatile INT32U  rdelay,wdelay;
    INT16U  i;
     feetDog();
     while(MARCSTATE != 0x01);
     LED_TX=0;
    rfBuffer.macID=MSG_BROADCAST;
    rf_send_packet((INT8U *) &rfBuffer, BUFFER_SIZE);     //
  //  UartSendString((INT8U *) &rfBuffer, BUFFER_SIZE);
  //  LED_TX=1;
    //每发射一次，闪烁一次。
  //  UartSendString("Send",4);   //leaf
    
     rfBuffer.msgType=MSG_TIME;
        for( rdelay = 0; rdelay < RETRY_TIMES; rdelay ++ )
    {
        for(i=0;i<sendNum;i++)
        { 
         // rfBuffer.macID=i;
          rfBuffer.macID=sendMac[i];
          LED_TX=0;
          rf_send_packet((INT8U *) &rfBuffer, BUFFER_SIZE);
          delay_nms(1); 
            LED_TX=1;
          for( wdelay = 0; wdelay < WAIT_TIMES; wdelay ++ )
        {
         feetDog();
          len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
          LED_TX=1;
          if( len!=0)   
          { 
         //   UartSendString((uchar *)buffer,7);
          
            getRfBuffer(buffer);
            if(rfBuffer.msgType==MSG_OK)
            {
              rfBuffer.msgType=MSG_SUCCESS;
              for(i=0;i<5;i++)
              {
                rf_send_packet((INT8U *) &rfBuffer, BUFFER_SIZE);
                delay_nms(2);
              }
              uartCmd.macID=rfBuffer.macID;
              uartCmd.msgType=UART_MSG_SUCCESS;
              UartSendString((uchar *)&uartCmd,BUFFER_SIZE);
              msgTimeCount=0;
              cmdFlag=FALSE;
              LED_RX=1;
              return;

            }
            }
          }
        }
    }
}
INT8U getUartCmd(void)
{
    
  int i;

//  uartData=0;
 
  if(uartFlag==1)
    {
      uartFlag=0;
      uartGet[uartCount++]=uartData;
      if(uartData=='#'||uartCount==15)  
      {
        uartCount=0;
        if(uartGet[2]==UART_MSG_FREQ)
        {  
          switch (uartGet[3])
          {
          case 0:
             rf_cc1110_init( RF_FREQ_433MHZ );
             break;
          case 1:
           rf_cc1110_init( RF_FREQ_868MHZ );
             break;
          case 2:
           rf_cc1110_init( RF_FREQ_902MHZ );
             break;             
          } 
          UartSendString("RF Changed",10);
          return FALSE;  
        }
        uartCmd.num=uartGet[1]*256+uartGet[0];
        uartCmd.msgType=UART_MSG_ACCESS;
        uartCmd.groupID=uartGet[3];
        uartCmd.macID=uartGet[4];
        uartCmd.tableID=uartGet[5];
        uartCmd.orderID=uartGet[6];
    //    uartCmd.flag=UART_MSG_ACCESS;
      //  uartGet[6]=UART_MSG_ACCESS;
        maxTimes=uartGet[7];
	uartCmd.idList.max=uartGet[8];	
        for(i=0;i<10;i++)
          uartCmd.idList.id[i]=uartGet[9+i];
         getMacId(&uartCmd.idList);
         
          UartSendString((uchar *)&uartCmd,8);
          getRfBuffer(uartGet);
       //   UartSendString((uchar *)&uartGet,BUFFER_SIZE);
        
        return TRUE;
      }else
      {
        return FALSE;
      }
    }
    return FALSE;
  }

#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{
	URX0IF=0;
        uartFlag=1;
	uartData=U0DBUF;

}
#pragma vector = RF_VECTOR
 __interrupt void RF_ISR1(void)
 {
    S1CON &= ~0x03;
 }
#pragma vector = ST_VECTOR
 __interrupt void ST_ISR(void)
 {
 	IRCON &=  ~0x80;
        WORIRQ &= ~0X01;
         SET_POWER_MODE(0);
 }
