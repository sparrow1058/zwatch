#include "ioCC1110.h"
#include "rf_cc1110.h"
#include "hal_cc1110.h"
#include "ap_define.h"
typedef struct RF_MASK                  //6 byte
{
  INT8U  max;
  INT8U  id[5];
}RFMASK;
typedef struct WA_RF_DATA          //17Byte
{
 // INT16U  num;
  INT8U   msgType;
  INT8U   groupID;
  INT8U   macID;
  INT8U   tableID[8];
  INT8U   orderID[4];
  INT16U   sum;
}WARFDATA;
typedef struct RE_RF_DATA                       //22 Byte
{
  INT16U  num;
  INT8U   msgType;
  INT8U   REID;
  INT8U   macID;  
  INT8U   tableID[8];
  INT8U   orderID[4];
  INT8U   max;
  RFMASK   idList; 
}RERFDATA;
typedef struct UART_CMD // 24byre
{
  INT16U  num;
  INT8U   msgType;
  INT8U   REID;
  INT8U   macID;  
  INT8U   tableID[8];
  INT8U   orderID[4];
  INT8U   max;
  RFMASK   idList;
 
}UARTCMD;               //19 byte
typedef struct RE_TO_AP_DATA
{
	INT16U 	num;
	INT8U	msgType;
	INT8U	REID;
	INT8U	macID;
}RETOAPDATA;
extern void InitUART(void);
extern void UartSendString(uchar *data,int len);
void handleWAMessage(void);
uchar handleREMessage(void);
void getTestData(void);
INT8U getUartCmd(void);
WARFDATA WASendBuffer;
RERFDATA RESendBuffer;
WARFDATA recBuffer;
RETOAPDATA	RERecBuffer;
UARTCMD uartCmd;
uchar uartData;
uchar uartCount=0;
uchar uartFlag=0;
uchar cmdFlag=0;
INT8U maxTimes=10;
INT8U RERetryTimes=5;
//volatile  INT8U  g_channel=0x00;
uchar uartGet[UART_CMD_SIZE];
uchar sendMac[40];
uchar sendNum;
uchar sendREID[8];
uchar sendREID[8];
uchar sendRENum;

#define UART_LOG        UartSendString("xxxx",4)

uchar  buffer[BUFFER_SIZE];
INT16U  msgTimeCount=0;
uchar g_channel[1]={0x03};
uchar apWorkMode=0;             //workMode 0.AP-WA   1.AP-RE 
uchar apTestMode=0;
uchar testData[24]={0x00,0x01,0x01,0xFF,0xFF,0x00,0x31,0x53,0xF7,0x53,0x05,0x53,0xa2,0x4e,0x70,0x53,0x55,0x14,0x08,0xFF,0x00,0x00,0x00,0x00};  //0031\u53f7\u5305\u53a2   //\u4e70\u5355
INT8U rfTest(void);
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
   //   rf_cc1110_init( RF_FREQ_433MHZ );
   // ioInit();
    IEN0 = 0x81;
        WORCTL |= 0x00;    //2^15 period
    WORIRQ |= 0X10;   //
 //   IEN0 |= 0X20;     //开中断
 //   InitWatchdog();             //leaf init watch dog  
}
void getREId(uchar id)
{
  uchar i,cnt=0;
  for(i=0;i<8;i++)
  {   if((id>>i)&0x01)
      sendREID[cnt++]=i+1;
  }
  sendRENum=cnt;
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
void getRERfBuffer(uchar  *buf)
{
    uchar *data=buf;
     uchar i; 
    uchar *pREBuffer=(uchar *)&RESendBuffer;
 for(i=0;i<RE_CMD_SIZE;i++)
 {
    *(pREBuffer++)=*(data++);
    
 } 
 getREId(RESendBuffer.REID);
    
}
void getWARfBuffer(uchar  *buf)
{
  INT8U *data=buf;
  uchar i;
  data=data+2;
 // rfBuffer.sum=CRC16(data,12);
  WASendBuffer.msgType=*(data++);
  WASendBuffer.groupID=*(data++);
  WASendBuffer.macID=*(data++);
  for (i=0;i<8;i++)
    WASendBuffer.tableID[i]=*(data++);
  for (i=0;i<4;i++)
    WASendBuffer.orderID[i]=*(data++);

}
INT8U getREBuffer(uchar  *buf)
{
	INT8U *data=buf;
	RERecBuffer.num=*(data+1)*256+*data;
	RERecBuffer.msgType=*(data+2);
	RERecBuffer.REID=*(data+3);
	RERecBuffer.macID=*(data+4);
	return RERecBuffer.msgType;
	
}
void getWABuffer(uchar  *buf)
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
  
  boardInit();
  InitUART();
  UartSendString("Zserver",7);
  while( 1 )
  {

    cmdFlag=getUartCmd();
    feetDog();

      while(cmdFlag==TRUE) 
     {
       SET_CHANNEL(g_channel[0]);
       rf_cc1110_init( RF_FREQ_433MHZ ); //Reinit 433 register
       msgTimeCount++;
       if(apWorkMode==0)
        handleWAMessage();
      else if(apWorkMode==1)
        handleREMessage();
       
       if(msgTimeCount>maxTimes)
      {
        msgTimeCount=0;
        cmdFlag=FALSE;
        LED_RX=LED_OFF;
        uartCmd.msgType=UART_MSG_FAIL;        
        UartSendString((uchar *)&uartCmd,BUFFER_SIZE+1);
        
      }
      uartFlag=0;
      uartData=0;
      uartCount=0;

     }
 //    SET_POWER_MODE(3);
  }    
}

#define RE_RETRY_TIMES		3
#define RE_REPLY_SIZE		6
#define RE_MAXTIMES			1000	
INT8U waitForRepeterReply(void)
{
		//			case AP_RFMSG_SUCCESS_RE:
	//			case AP_RFMSG_FAIL_RE:
	INT8U rssi, lqi, len,ret;
	INT16U timeoutCnt=0;
	feetDog();
	//UartSendString("wwwwwww",7);
	while(MARCSTATE != 0x01);
	while(1)
	{
		feetDog();
		len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
		if(len==RE_REPLY_SIZE)
		{
			
			//LED_RX=LED_ON;
			LED_TX=LED_OFF;
			ret=getREBuffer(buffer);
                        
			if(ret==AP_RFMSG_FAIL_RE||ret==AP_RFMSG_SUCCESS_RE)
                          break;
		}
		if(timeoutCnt++>RE_MAXTIMES)
		{
			timeoutCnt=0;	
             
			ret= AP_RFMSG_TIMEOUT;
			break;
		}
	}
        LED_RX=LED_OFF;
        UartSendString((uchar*)&RERecBuffer,6);  
        return ret;
	
}
INT8U handleREMessage(void)
{
    INT8U rssi, lqi, len,reMsgType=0;
    volatile INT32U  rdelay,wdelay;
	INT8U result;
	INT16U  i;
	feetDog();
	while(MARCSTATE != 0x01);
	for( rdelay = 0; rdelay < RERetryTimes; rdelay ++ )
       {
		for(i=0;i<sendRENum;i++)
		{

			RESendBuffer.msgType=AP_RFMSG_TIME_RE;
			RESendBuffer.REID=sendREID[i];
            LED_TX=LED_ON;
		//    UartSendString((INT8U *)&RESendBuffer,RE_CMD_SIZE);
			rf_send_packet((INT8U *) &RESendBuffer, RE_CMD_SIZE);
			delay_nms(1); 
            LED_TX=LED_OFF;
          for( wdelay = 0; wdelay < 5; wdelay ++ )
          {
           feetDog();
           len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
            LED_TX=LED_OFF;
            if(len==RE_REPLY_SIZE)
              {

                  LED_RX=LED_ON;
                  LED_TX=LED_OFF;
                  reMsgType=getREBuffer(buffer);
  		if((RESendBuffer.REID==RERecBuffer.REID)&&(reMsgType==AP_RFMSG_ACCESS_RE))
                {

                  break;
                }
             
             

           }
          }
                      
                if(reMsgType==AP_RFMSG_ACCESS_RE)
                {
                  reMsgType=0;
                  result=waitForRepeterReply();
                  msgTimeCount=0;
                  cmdFlag=FALSE;
                  LED_RX=LED_OFF;
                  if(result==AP_RFMSG_SUCCESS_RE)
                    return result;
                }
                        
        }
                
	}
        return result;
  
}
void handleWAMessage(void)
{
      INT8U rssi, lqi, len;
    
    volatile INT32U  rdelay,wdelay;
	INT16U  i;
	feetDog();
	while(MARCSTATE != 0x01);
	LED_TX=LED_ON;
  for( rdelay = 0; rdelay < RETRY_TIMES; rdelay ++ )
    {
      
        for(i=0;i<sendNum;i++)
        { 
         // rfBuffer.macID=i;
          WASendBuffer.msgType=MSG_TIME;
          WASendBuffer.macID=sendMac[i];
          LED_TX=LED_ON;
          WASendBuffer.sum=CRC16((INT8U *)&WASendBuffer,BUFFER_SIZE-2);
          rf_send_packet((INT8U *) &WASendBuffer, BUFFER_SIZE);

          delay_nms(1); 
            LED_TX=LED_OFF;
        for( wdelay = 0; wdelay < 2; wdelay ++ )
        {
         feetDog();
        // for(i=0;i<3;i++)
         { 

           len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
			LED_TX=LED_OFF;		
           

         }
         // if( len!=0)   
         if(len==BUFFER_SIZE) 
         { 
			LED_RX=LED_ON;
            getWABuffer(buffer);
            if(recBuffer.msgType==MSG_ACCESS)
            {
              
            }
            if(recBuffer.msgType==MSG_SUCCESS)
            {

              if(recBuffer.orderID[0]!=WASendBuffer.orderID[0])
                return ;
             
              for(i=0;i<3;i++)
              {
                 recBuffer.sum=CRC16((INT8U *)&recBuffer,BUFFER_SIZE-2);
                rf_send_packet((INT8U *) &recBuffer, BUFFER_SIZE);
                delay_nms(5);
              }
              uartCmd.macID=recBuffer.macID;
              uartCmd.msgType=UART_MSG_SUCCESS;
              uartCmd.max=msgTimeCount;
              UartSendString((uchar *)&uartCmd,BUFFER_SIZE+1);
              msgTimeCount=0;
              cmdFlag=FALSE;
              LED_RX=LED_OFF;
              return;

            }
            }
          }
        }
    }
}
void getUartData(void)
{
          
        int i;
        uartCmd.num=uartGet[RF_NUM+1]*256+uartGet[RF_NUM];
       // uartCmd.msgType=UART_MSG_ACCESS;
        uartCmd.REID=uartGet[RF_REID];
        if(uartCmd.REID!=0&&uartCmd.REID!=0xFF)
        {
            apWorkMode=1; //ap-RE Mode
           // UartSendString("xxxx",4);
           getRERfBuffer(uartGet);
           RERetryTimes=uartGet[RF_MACID];
           uartGet[RF_MSG]= AP_RFMSG_ACCESS_RE;
           UartSendString((uchar *)&uartGet,BUFFER_SIZE+1);
            
 
        }else
        {      
            apWorkMode=0; //ap-WA Mode
        for(i=0;i<CHAR_NUM;i++)
          uartCmd.tableID[i]=uartGet[RF_TABLE+i];        //Copy the utf8 code
        for(i=0;i<4;i++)
          uartCmd.orderID[i]=uartGet[RF_ORDER+i];        //Copy the utf8 code
        maxTimes=uartGet[RF_SUM];
        uartCmd.max=uartGet[RF_SUM];
		uartCmd.idList.max=uartGet[RF_IDLIST];	
        for(i=0;i<10;i++)
          uartCmd.idList.id[i]=uartGet[RF_IDLIST+1+i];
         getMacId(&uartCmd.idList);
         uartCmd.msgType=UART_MSG_ACCESS;
          UartSendString((uchar *)&uartCmd,BUFFER_SIZE+1);
                         
          getWARfBuffer(uartGet);
        }
 }
void getTestData(void)
{
    
        int i;
        uartCmd.num=testData[RF_NUM+1]*256+testData[RF_NUM];
        uartCmd.msgType=UART_MSG_ACCESS;
        uartCmd.REID=testData[RF_REID];
        uartCmd.macID=testData[RF_MACID];

        for(i=0;i<CHAR_NUM;i++)
          uartCmd.tableID[i]=testData[RF_TABLE+i];        //Copy the utf8 code
        for(i=0;i<4;i++)
          uartCmd.orderID[i]=testData[RF_ORDER+i];        //Copy the utf8 code

        maxTimes=testData[RF_SUM];
		uartCmd.idList.max=testData[RF_IDLIST];	
        for(i=0;i<10;i++)
          uartCmd.idList.id[i]=testData[RF_IDLIST+1+i];
         getMacId(&uartCmd.idList);
          UartSendString((uchar *)&uartCmd,BUFFER_SIZE);
          getWARfBuffer(testData);
       

  }

INT8U getUartCmd(void)
{
    

  int ret=0;
 // uartData=0;
#ifdef RF_TEST  
  if(KEY1==0)
      apTestMode=1;
  if(apTestMode==1)
  {
    getTestData();
    return 1;
  }
#endif  
  if(uartFlag==1)
    {
      uartFlag=0;
      uartGet[uartCount++]=uartData;
      if(uartData=='#'||uartCount==UART_CMD_SIZE)  
      {
        uartCount=0;
        switch (uartGet[2])
        {
        case UART_MSG_STATUS: 
              break;
        case UART_MSG_FREQ_STATUS:
             uartGet[3]=g_channel[0];
             UartSendString((uchar *)&g_channel,1);
              break;
        case UART_MSG_FREQ_CHANGE:
              g_channel[0]=uartGet[3];
              UartSendString((uchar *)&uartGet,4);
              break;
        case UART_MSG_TEST:
              getTestData();
              ret=1;
              break; 
        case UART_MSG_ORDER:
             getUartData();
              ret=1;
              break;      
        default:
              break;
        }
      }
    }
    return ret;
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
