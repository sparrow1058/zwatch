/*****************************************************************************

*��Ȩ��Ϣ��������΢�������޹�˾
*�� �� ����TM1651-V1.0
*��ǰ�汾��V1.0
*MCU �ͺţ�STC12C5608AD
*����������Keil uVision4
*����Ƶ�ʣ�11.0592MHZ      
*������ڣ�2013-07-19
*�����ܣ�����������Ͱ���������7��4λLED�����������ʾ,����Ӧ��������ʱ��ʾ1~7,ԭ��ͼ��ο�TM1651����飻         
*����������1.�˳���ΪTM1651��������LED����ܺͰ�����ʾ���򣬽����ο�֮�á�
            2.����ֱ��ʹ�ñ����̳�����ɾ�����ʧ�ģ�����˾���е��κ�����            
********************************************************************************/
#include <iocc1110.h>
#include "mytypedef.h"

#define nop asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");//�궨��


/********************������ƶ˿�**********************/
#define CLK P1_2 //����CLK
#define  DIO P1_3 //����DIO
#define DEFAULT_VAL     0x8B

/********************��������*************************/
//unsigned char code CODE[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,};//���������0~9������
//unsigned char code TAB[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};//����ܲ���ʾ

/********************��ʱ��������ʱnms******************/
void delay_nms(unsigned int n)
{
	unsigned int i;
	while(n--)
	   for(i=0;i<550;i++);
}

/********************Start����*************************/
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

/********************Stop����*************************/
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

/***************����8bit���ݣ��ӵ�λ��ʼ**************/
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
		  //8λ���ݴ�����
		CLK=0;//�ж�оƬ��������ACKӦ���ź�
		nop;
                
		while(DIO==1);
		nop;
		CLK=1;
		nop;
}


void TMCloseAll()
{
			I2CStart();
		I2CWritebyte(0x80); //��ʾ�����������ʾ��������Ϊ11/16.
		I2CStop();
}
/************��ʾ��������ַ�Լ�һ************/
void TMShowAuto(uchar * numArray)                    
{
		unsigned char i;
		I2CStart();
		I2CWritebyte(0x40);  //�����������ã���ַ�Զ���1��д���ݵ���ʾ�Ĵ���
		I2CStop();

		I2CStart();
		I2CWritebyte(0xc0);  //��ַ�������ã���ʼ��ַΪ00H
		for(i=0;i<4;i++)  //����4�ֽ����ݵ��Դ�
		{
		I2CWritebyte(*numArray);
		numArray++;
		}
		I2CStop();

		I2CStart();
		I2CWritebyte(DEFAULT_VAL); //��ʾ�����������ʾ��������Ϊ11/16.
		I2CStop();

}

/************��ʾ�������̶���ַд����************/
void disp(unsigned char add, unsigned char value)
{
		I2CStart();
		I2CWritebyte(0x44); //�����������ã��̶���ַ��д���ݵ���ʾ�Ĵ���
		I2CStop();

		I2CStart();
		I2CWritebyte(add);//��ַ�������ã�д��add��Ӧ��ַ

		I2CWritebyte(value);//��add��ַд����
		I2CStop();

		I2CStart();
		I2CWritebyte(0x8A);//��ʾ�����������ʾ��������Ϊ11/16.
		I2CStop();

}
