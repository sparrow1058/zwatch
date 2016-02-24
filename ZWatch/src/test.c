

#include "ioCC1110.h"
#include "rf_cc1110.h"

#define MYADDR		0x01

#define LEDON  1
#define LEDOFF 0
#define CHG_DET		P1_4
#define PWR_KEY		P1_5
#define MOTO_DRV	P1_6
#define SPK_DRV		P1_7
#define PWR_DRV         P2_0

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


//函数声明
typedef struct LED_INFO
{
  uchar led0;
  uchar led1;
  uchar order;
  uchar rgb;
}LEDINFO;

extern void TMShow();
extern void TMShowAuto(uchar *numArray);
extern void TMDisplaySet(uchar num,uchar cmd);
extern  void TMCloseAll();

uchar ledCode[9]={0x00,0x00,0x04,0x40,0x20,0x02,0x08,0x10,0x01};
//uchar CODE[10] = {0xC0,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
uchar digCode[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
//uchar LedBuffer[4]={0x3f,0x06,0x3f,0x3F};
LEDINFO ledBuffer;
INT32U   PowerCount;
uchar ledFlag=0;
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
void boardInit()
{
    
    P1DIR |= 0x03;                //P1_0, p1_1, OUTPUT
  
    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
    

    rf_cc1110_init( 433000 );
    ioInit();
    ledBuffer.led0=0;
    ledBuffer.led1=1;
    ledBuffer.order=0x01;
    ledBuffer.rgb=0x02;
    IEN0 = 0x81;
}


    int main( void )
{
    INT8U buffer[10], rssi, lqi;

    boardInit();
   TMShowLedInfo(&ledBuffer);
    while( 1 ){
      if(PWR_KEY==1)
      {
         P1_0=!P1_0;
        ledFlag=!ledFlag;
      }
    
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
     if( len !=0 )
     {
         P1 ^= 0x03    ;        //收到数据，LED翻转一次
         rf_send_packet( buffer, 10 );//发送应答信号
      //if(ledFlag)
      //   
     // else
      //  TMCloseAll();
      //  TMShowLedInfo(&ledBuffer);
     }
     
     len = 0;
    }
  
  
  

}