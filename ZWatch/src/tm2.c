/*****************************************************************************

*版权信息：深圳天微电子有限公司
*文 件 名：TM1651-V1.0
*当前版本：V1.0
*MCU 型号：STC12C5608AD
*开发环境：Keil uVision4
*晶震频率：11.0592MHZ      
*完成日期：2013-07-19
*程序功能：数码管驱动和按键：驱动7段4位LED共阳数码管显示,当对应按键按下时显示1~7,原理图请参考TM1651规格书；         
*免责声明：1.此程序为TM1651驱动共阳LED数码管和按键演示程序，仅作参考之用。
            2.如有直接使用本例程程序造成经济损失的，本公司不承担任何责任            
********************************************************************************/
#include <iocc1110.h>
#include "mytypedef.h"

#define nop asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");//宏定义


/********************定义控制端口**********************/
#define CLK P1_2 //定义CLK
#define  DIO P1_3 //定义DIO
#define DEFAULT_VAL     0x8B

/********************定义数据*************************/
//unsigned char code CODE[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,};//共阳数码管0~9字型码
//unsigned char code TAB[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};//数码管不显示

/********************延时函数，延时nms******************/
void delay_nms(unsigned int n)
{
	unsigned int i;
	while(n--)
	   for(i=0;i<550;i++);
}

/********************Start函数*************************/
void I2CStart()
{
		DIO=1;
		CLK=1;
		nop;
		DIO=1;
		nop;
		DIO=0;
		nop;
		CLK=0;
}

/********************Stop函数*************************/
void I2CStop()
{
		CLK=0;
		nop;
		nop;
		DIO=0;
		nop;
		nop;
		CLK=1;
		nop;
		nop;
		nop;
		DIO=1;
		nop;
		CLK=0;
		DIO=0;
}

/***************发送8bit数据，从低位开始**************/
void I2CWritebyte(unsigned char oneByte)
{
		  unsigned char i;
		  for(i=0;i<8;i++)
		  {
			CLK=0;
		if(oneByte&0x01)
		  DIO=1;
		else
		  DIO=0;
		nop;
                nop;nop;nop;
			CLK=1;
			oneByte=oneByte>>1;
                        nop;nop;nop;nop;
		  }
		  //8位数据传送完
		CLK=0;//判断芯片发过来的ACK应答信号
		nop;
                
		while(DIO==1);
		nop;
		CLK=1;
		nop;
}


void TMCloseAll()
{
			I2CStart();
		I2CWritebyte(0x80); //显示控制命令：开显示，脉冲宽度为11/16.
		I2CStop();
}
/************显示函数，地址自加一************/
void TMShowAuto(uchar * numArray)                    
{
		unsigned char i;
		I2CStart();
		I2CWritebyte(0x40);  //数据命令设置：地址自动加1，写数据到显示寄存器
		I2CStop();

		I2CStart();
		I2CWritebyte(0xc0);  //地址命令设置：初始地址为00H
		for(i=0;i<4;i++)  //发送4字节数据到显存
		{
		I2CWritebyte(*numArray);
		numArray++;
		}
		I2CStop();

		I2CStart();
		I2CWritebyte(DEFAULT_VAL); //显示控制命令：开显示，脉冲宽度为11/16.
		I2CStop();

}

/************显示函数，固定地址写数据************/
void disp(unsigned char add, unsigned char value)
{
		I2CStart();
		I2CWritebyte(0x44); //数据命令设置：固定地址，写数据到显示寄存器
		I2CStop();

		I2CStart();
		I2CWritebyte(add);//地址命令设置：写入add对应地址

		I2CWritebyte(value);//给add地址写数据
		I2CStop();

		I2CStart();
		I2CWritebyte(0x8A);//显示控制命令：开显示，脉冲宽度为11/16.
		I2CStop();

}
