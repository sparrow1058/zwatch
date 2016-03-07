

#include "ioCC1110.h"
#include "rf_cc1110.h"

#define MYADDR		0x01
#define BROADCAST   0xFF

#define MSG_TIME      0x01
#define MSG_SUCCESS      0x02
#define MSG_BROADCAST 0xFF
#define MSG_OK		0x08

#define TIMEOUT     150    //20s
#define KEY_LONG_PRESS  15
#define KEY_LED_DELAY 2
#define RF_LENGTH   6

#define CHG_DET		P1_4
#define PWR_KEY		P1_5
#define MOTO_DRV	P1_6
#define SPK_DRV		P1_7
#define PWR_DRV         P2_0
#define SEG_RGB       3
#define SEG_LED       2
#define R_VAL       1
#define G_VAL       2
#define B_VAL       4



/*
///Dig 4
#define LED_R	0x01
#define LED_G	0x02
#define LED_B	0x04
#define LED1	0x08		//Dig 4
///Dig 3
#define	LED2	0x04
#define	LED3	0x40
#define	LED4	0x20
#define	LED5	0x02
#define	LED6	0x08
#define	LED7	0x10
#define	LED8	0x01
*/


typedef struct RF_DATA
{
  INT16U  num;
  INT8U   msgType;
  INT8U   macID;
  INT8U   tableID;
  INT8U   orderID;
  INT8U   sum;
}RFDATA;

//函数声明
typedef struct LED_INFO
{
  uchar led0;
  uchar led1;
  uchar order;
  uchar rgb;
}LEDINFO;

//extern void TMShow();
void TMShow(uchar num,uchar data);
extern void TMShowAuto(uchar *numArray);
extern void TMClose(uchar num);
extern  void TMCloseAll();
extern void UartSendString(uchar *data,int len);
extern void InitUART(void);
void halWait(INT8U wait);

uchar ledCode[9]={0x00,0x00,0x04,0x40,0x20,0x02,0x08,0x10,0x01};
//uchar CODE[10] = {0xC0,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
uchar digCode[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
//uchar LedBuffer[4]={0x3f,0x06,0x3f,0x3F};
LEDINFO ledBuffer;
RFDATA  rfBuffer;
INT32U   PowerCount;
uchar ledFlag=0;
uchar keyFlag=0;
uchar successFlag=0;
uchar keyCount=0;
uchar keyDelay=0;
uchar keyDown=0;
uchar keyUp=1;
uchar keyLed=0;
uchar myTime=0;
uchar recCount;
INT32U  timeCount=0;
volatile INT8U len;

void ioInit()
{
        P0DIR = 0X3C; //P02-P05 OUT for select 8-led
	P1DIR = 0xCF; //
        P1INP |=0x20; 
        //P1SEL |=0x0F;
        P2DIR =0x01;
}
void TMShowLedInfo(LEDINFO *buffer)
{
  uchar tmp[4];
  tmp[0]=digCode[buffer->led0];
  tmp[1]=digCode[buffer->led1];
  tmp[2]=ledCode[buffer->order];
  tmp[3]=buffer->rgb;
  if(buffer->order==1)
      tmp[3]|=0x08;
  TMShowAuto(tmp);
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
void boardInit()
{
    
    P1DIR |= 0x03;                //P1_0, p1_1, OUTPUT
  
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
    
    ledBuffer.led0=0x08;
       ledBuffer.led1=0x08;
       ledBuffer.order=0x03; 
       ledBuffer.rgb=R_VAL;
    rf_cc1110_init( 433000 );
    ioInit();
    IEN0 = 0x81;
}


    int main( void )
{
    INT8U buffer[10], rssi, lqi;
    INT8U msgFlag;
    boardInit();
    InitUART();
    UartSendString("zwatch",6);
   //MCloseAll();
    timeCount=TIMEOUT-4;
   TMShowLedInfo(&ledBuffer);
    // TMShow(SEG_RGB,G_Val);
    while( 1 ){
      timeCount++;
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
      
      if(len!=0)
      {
        getRfBuffer(buffer);
        UartSendString((uchar *)buffer,7);
        UartSendString("###",3);
        if(rfBuffer.macID==BROADCAST)
        {  
             
            ledBuffer.led0=rfBuffer.tableID/10;
             ledBuffer.led1=rfBuffer.tableID%10;
             ledBuffer.order=rfBuffer.orderID; 
             ledBuffer.rgb=R_VAL;
             TMShowLedInfo(&ledBuffer);
              msgFlag=1;
              keyFlag=0;
			  timeCount=0;
        }
         if(rfBuffer.msgType==MSG_TIME)
            {
                  if(rfBuffer.macID==MYADDR)  
                          myTime=1;
                  else
                          myTime=0;
            }
         if(rfBuffer.macID==MYADDR&&rfBuffer.msgType==MSG_SUCCESS)
         {
             myTime=0;
             msgFlag=0;
             ledBuffer.rgb=G_VAL;    //rgb buffer;
             TMShowLedInfo(&ledBuffer);
             timeCount=TIMEOUT-10;
              UartSendString("SUCCESS",7);
            // TMCloseAll();
         }
      }
      if(PWR_KEY==1)
      {
        keyCount++;
        //keyFlag=1;
        keyDown=1;
        keyUp=0;
        if(myTime==0)
        {
          keyLed=1;
          
          TMShow(SEG_RGB,R_VAL);

        //  TMShowLedInfo(&ledBuffer);
        }
       }else 
       {
        keyUp=1;
        if(keyDown&&keyUp)
        {
          keyFlag=1;
          keyDown=0;
        }
         keyCount=0;
       }
      if(keyFlag&myTime&msgFlag)
      {
        keyFlag=0;
        rfBuffer.msgType=MSG_OK;
        UartSendString("MSG_OK",7);
        rf_send_packet((INT8U*) &rfBuffer, 10 );//发送应答信号
        
      
      } 
      if(timeCount>TIMEOUT)
      {
         timeCount=0;   
         myTime=0;
           msgFlag=0;
          TMCloseAll();
      }
      if(keyLed>0)
      { 
        keyLed++;
         if(keyLed>KEY_LED_DELAY&&myTime==0)
         {  
           TMClose(SEG_RGB);
           keyLed=0;
         }
       }
  /*
      if(keyCount>KEY_LONG_PRESS)
      {      
        keyCount=0;
        TMCloseAll();
        P2INP|=0x01;
       // while(1);
   //   void halWait(INT8U wait)
      halWait(100);
      }
   */
    }
    
}
