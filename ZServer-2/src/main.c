#include "ioCC1110.h"
#include "rf_cc1110.h"
#include "hal_cc1110.h"
#define RF_FREQ_433MHZ  433000
#define RF_FREQ_868MHZ  868000
#define RF_FREQ_902MHZ  902000



#define MAX_NUM     5
#define RETRY_TIMES   2
#define WAIT_TIMES    1
#define MSG_TIMEOUT     10
// msg type 
#define BROADCAST   0xFF

#define MSG_TIME      0x01
#define MSG_ACCESS      0x02
#define MSG_SUCCESS      0x03

#define MSG_BROADCAST   0xFF
#define MSG_OK		0x08


#define UART_MSG_ORDER    1
#define UART_MSG_ACCESS   2
#define UART_MSG_SUCCESS  3
#define UART_MSG_FAIL     4
#define UART_MSG_FREQ     0xFE    //change FREQ
#define UART_CMD_SIZE   24
#define BUFFER_SIZE   17
//machine ID
#define LED_TX    P1_0
#define LED_RX    P1_1

#define KEY1    P1_2
#define KEY2    P1_3
//External functions
/*** Define Data iD */

#define CHAR_NUM        4*2
#define RF_NUM          0
#define RF_MSG          RF_NUM+2
#define RF_GROUP        RF_MSG+1
#define RF_MACID        RF_GROUP+1
#define RF_TABLE        RF_MACID+1
#define RF_ORDER        RF_TABLE+CHAR_NUM
#define RF_SUM          RF_ORDER+4
#define RF_IDLIST       RF_SUM+1




typedef struct RF_MASK
{
  INT8U  max;
  INT8U  id[5];
}RFMASK;
typedef struct RF_DATA          //14 Byte
{
 // INT16U  num;
  INT8U   msgType;
  INT8U   groupID;
  INT8U   macID;
  INT8U   tableID[8];
  INT8U   orderID[4];
  INT16U   sum;
}RFDATA;
typedef struct UART_CMD // 21
{
  INT16U  num;
  INT8U   msgType;
  INT8U   groupID;
  INT8U   macID;  
  INT8U   tableID[8];
  INT8U   orderID[4];
  INT8U   max;
  RFMASK   idList;
 
}UARTCMD;               //19 byte
extern void InitUART(void);
extern void UartSendString(uchar *data,int len);
void handleMessage(void);
INT8U getUartCmd(void);
RFDATA sendBuffer;
RFDATA recBuffer;
UARTCMD uartCmd;
uchar uartData;
uchar uartCount=0;
uchar uartFlag=0;
uchar cmdFlag=0;
INT8U maxTimes=10;

uchar uartGet[UART_CMD_SIZE];
uchar sendMac[40];
uchar  buffer[BUFFER_SIZE];
uchar sendNum;
INT16U  msgTimeCount=0;

  
INT16U CRC16(uchar* pchMsg, uchar wDataLen)
 {          
   INT16U wCRCTalbeAbs[] = {  0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400};         
   INT16U wCRC = 0xFFFF;         
            INT16U i;         
            uchar chChar;          
            for (i = 0; i < wDataLen; i++)
            {                  
                               chChar = *pchMsg++;
                               wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
                               wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
            }          
            return wCRC; 
 }
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
void getUartRfBuffer(uchar  *buf)
{
  INT8U *data=buf;
  uchar i;
  data=data+2;
 // rfBuffer.sum=CRC16(data,12);
  sendBuffer.msgType=*(data++);
  sendBuffer.groupID=*(data++);
  sendBuffer.macID=*(data++);
  for (i=0;i<8;i++)
    sendBuffer.tableID[i]=*(data++);
  for (i=0;i<4;i++)
    sendBuffer.orderID[i]=*(data++);

}
void getRfBuffer(uchar  *buf)
{
  INT8U *data=buf;
  uchar i;
 // data=data+2;
 // rfBuffer.sum=CRC16(data,12);
  recBuffer.msgType=*(data++);
  recBuffer.groupID=*(data++);
  recBuffer.macID=*(data++);
  for (i=0;i<8;i++)
    recBuffer.tableID[i]=*(data++);
  for (i=0;i<4;i++)
    recBuffer.orderID[i]=*(data++);

}
int main( void )
{
  int i;   
  boardInit();
     InitUART();
     UartSendString("Zserver",7);
  
  while( 1 )
  {
    
    cmdFlag=getUartCmd();
   feetDog();
      while(cmdFlag==TRUE) 
     {
      
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
      uartData=0;
      uartCount=0;

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
   // UartSendString((uchar *)&sendMac,sendNum);
  //  UartSendString("00000",5);

  
}

void handleMessage(void)
{
      INT8U rssi, lqi, len;
    
    volatile INT32U  rdelay,wdelay;
    INT16U  i;
     feetDog();
     while(MARCSTATE != 0x01);
     LED_TX=0;
  //  rfBuffer.macID=MSG_BROADCAST;
  //  rf_send_packet((INT8U *) &rfBuffer, BUFFER_SIZE);     //
   // UartSendString((INT8U *) &rfBuffer, BUFFER_SIZE);
  //  LED_TX=1;
    //每发射一次，闪烁一次。
  // UartSendString("Send",4);   //leaf
    
     
        for( rdelay = 0; rdelay < RETRY_TIMES; rdelay ++ )
    {
      
        for(i=0;i<sendNum;i++)
        { 
         // rfBuffer.macID=i;
          sendBuffer.msgType=MSG_TIME;
          sendBuffer.macID=sendMac[i];
          LED_TX=0;
          sendBuffer.sum=CRC16((INT8U *)&sendBuffer,BUFFER_SIZE-2);
          rf_send_packet((INT8U *) &sendBuffer, BUFFER_SIZE);
          UartSendString((INT8U *) &sendBuffer, BUFFER_SIZE);   //leaf
       //   UartSendString(0x0d, 1);   //leaf
          delay_nms(1); 
            LED_TX=1;
        for( wdelay = 0; wdelay < WAIT_TIMES; wdelay ++ )
        {
         feetDog();
        // for(i=0;i<3;i++)
         { 
           len = rf_rec_packet(buffer, &rssi, &lqi, 500) ;
           //delay_nms(10);
           //if(len==14)break; 
           LED_TX=1;
        //   if(len!=0)
          //  UartSendString((uchar *)buffer,len);
         }
         // if( len!=0)   
         if(len==BUFFER_SIZE) 
         { 
         //   UartSendString((uchar *)buffer,BUFFER_SIZE);
            getRfBuffer(buffer);
            if(recBuffer.msgType==MSG_ACCESS)
            {
              
            }
            if(recBuffer.msgType==MSG_SUCCESS)
            {
           //  UartSendString("SUCCESS",5);
              if(recBuffer.orderID[0]!=sendBuffer.orderID[0])
                return ;
             
              for(i=0;i<3;i++)
              {
                 recBuffer.sum=CRC16((INT8U *)&recBuffer,BUFFER_SIZE-2);
                rf_send_packet((INT8U *) &recBuffer, BUFFER_SIZE);
                delay_nms(5);
              }
              uartCmd.macID=recBuffer.macID;
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

 // uartData=0;
 
  if(uartFlag==1)
    {
      uartFlag=0;
      uartGet[uartCount++]=uartData;
      if(uartData=='#'||uartCount==UART_CMD_SIZE)  
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
       // UartSendString((uchar *)&uartGet,BUFFER_SIZE);
        uartCmd.num=uartGet[RF_NUM+1]*256+uartGet[RF_NUM];
        uartCmd.msgType=UART_MSG_ACCESS;
        uartCmd.groupID=uartGet[RF_GROUP];
        uartCmd.macID=uartGet[RF_MACID];
        //uartCmd.tableID=uartGet[5];
        for(i=0;i<CHAR_NUM;i++)
          uartCmd.tableID[i]=uartGet[RF_TABLE+i];        //Copy the utf8 code
        for(i=0;i<4;i++)
          uartCmd.orderID[i]=uartGet[RF_ORDER+i];        //Copy the utf8 code
       // uartCmd.orderID=uartGet[RF_ORDER];
    //    uartCmd.flag=UART_MSG_ACCESS;
      //  uartGet[6]=UART_MSG_ACCESS;
        maxTimes=uartGet[RF_SUM];
	uartCmd.idList.max=uartGet[RF_IDLIST];	
        for(i=0;i<10;i++)
          uartCmd.idList.id[i]=uartGet[RF_IDLIST+1+i];
         getMacId(&uartCmd.idList);
         
          UartSendString((uchar *)&uartCmd,BUFFER_SIZE);
                         
          getUartRfBuffer(uartGet);
        //  UartSendString((uchar *)&rfBuffer,BUFFER_SIZE);
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
