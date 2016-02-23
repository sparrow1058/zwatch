//定义端口
#include <iocc1110.h>
#include "mytypedef.h"
#define clk P1_2
#define dio P1_3

///=======================================
void Delay_us(BYTE i) //nus 延时
{
        for(; i>0; i--)
        {
               asm("NOP");               
              asm("NOP");                       
        }        
}

///======================================
void I2CStart(void) // 1651 开始
{
        clk = 1;
        dio = 1;
        Delay_us(2);
        dio = 0;
}

//=============================================
void I2Cask(void) //1651 应答
{
        clk = 0;
        Delay_us(5); //在第八个时钟下降沿之后延时5us，开始判断ACK 信号
        while(dio);
        clk = 1;
        Delay_us(2);
        clk=0;
}

//========================================
void I2CStop(void) // 1651 停止
{
        clk = 1;
        dio = 0;
        Delay_us(2);
        dio = 1;
}

//=========================================
void I2CWrByte(BYTE oneByte) //写一个字节
{
        BYTE i;

        for(i=0;i<8;i++)
        { 
                clk = 0;
                if(oneByte&0x01) //低位在前
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
BYTE GetKey(void) //读按键
{
        BYTE rekey, i;

        I2CStart();
        I2CWrByte(0x46); //读按键命令
        I2Cask();

        dio=1; // 在读按键前拉高数据线
        for(i=0;i<8;i++) //从低位开始读
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