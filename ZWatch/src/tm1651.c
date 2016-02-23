//����˿�
#include <iocc1110.h>
#include "mytypedef.h"
#define clk P1_2
#define dio P1_3

///=======================================
void Delay_us(BYTE i) //nus ��ʱ
{
        for(; i>0; i--)
        {
               asm("NOP");               
              asm("NOP");                       
        }        
}

///======================================
void I2CStart(void) // 1651 ��ʼ
{
        clk = 1;
        dio = 1;
        Delay_us(2);
        dio = 0;
}

//=============================================
void I2Cask(void) //1651 Ӧ��
{
        clk = 0;
        Delay_us(5); //�ڵڰ˸�ʱ���½���֮����ʱ5us����ʼ�ж�ACK �ź�
        while(dio);
        clk = 1;
        Delay_us(2);
        clk=0;
}

//========================================
void I2CStop(void) // 1651 ֹͣ
{
        clk = 1;
        dio = 0;
        Delay_us(2);
        dio = 1;
}

//=========================================
void I2CWrByte(BYTE oneByte) //дһ���ֽ�
{
        BYTE i;

        for(i=0;i<8;i++)
        { 
                clk = 0;
                if(oneByte&0x01) //��λ��ǰ
                {
                        dio = 1;
                }
                else
                {
                        dio = 0;
                }                
                Delay_us(3);
                oneByte = oneByte>>1;
                clk = 1;
                Delay_us(3);
        }
}
void TMShow(uchar num)
{
  I2CStart();
  I2CWrByte(0x44);
  I2Cask();
  I2CStop();  
   I2CStart();
  I2CWrByte(0xC0);
  I2Cask();
  I2CWrByte(num);
   I2Cask();
   I2CStop(); 
   I2CStart();
    I2CWrByte(0x88);    
   I2Cask();
   I2CStop(); 

}

void TMDisplaySet(uchar num,uchar cmd)
{
   I2CStart();
  I2CWrByte(0x40);    //auto mode
   I2Cask();
   I2CStop();  
   I2CStart();
   I2CWrByte(0xC0+num);
   I2Cask();
   I2CStop(); 
   I2CStart();
   if(cmd)
        I2CWrByte(0x8F);    
     else
        I2CWrByte(0x80);    
   I2Cask();
   I2CStop();
}
void TMShowAuto(uchar * numArray)
{
  uchar *tmp=numArray;
   I2CStart();
  I2CWrByte(0x40);    //auto mode
   I2Cask();
   I2CStop();  
   I2CStart();
   I2CWrByte(0xC0);
   I2Cask();
   for(int i=0;i<4;i++)
   {
     I2CWrByte(*tmp++);
    I2Cask();
   }
   I2CStop(); 
   I2CStart();
   I2CWrByte(0x8F);    
   I2Cask();
   I2CStop();
}
void TMCloseAll()
{
       TMDisplaySet(0,0);
       TMDisplaySet(1,0);
       TMDisplaySet(2,0);
       TMDisplaySet(3,0);
}
//-------------------------------------------------
BYTE GetKey(void) //������
{
        BYTE rekey, i;

        I2CStart();
        I2CWrByte(0x46); //����������
        I2Cask();

        dio=1; // �ڶ�����ǰ����������
        for(i=0;i<8;i++) //�ӵ�λ��ʼ��
        { 
                clk = 0;
                rekey = rekey>>1;
                Delay_us(30);
                clk = 1;
                if(dio)
                {
                        rekey = rekey|0x80;
                }
                else
                {
                        rekey = rekey|0x00;
                }
                Delay_us(30);
        }

        I2Cask();
        I2CStop();
        return (rekey);
}