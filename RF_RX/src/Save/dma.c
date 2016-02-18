//DMA.c
#include "hal.h"
DMA_DESC /*__xdata*/		dmaChannel[4];
DMA_TABLE_ENTRY /*__xdata*/ dmaTable[4];
unsigned char recbuff[10];
unsigned char sendbuff[10];
void delayus(unsigned int u)
{
    unsigned int  i;
    for(i=0;i<u*13;i++);
}
void DMA_setup(uchar ch)
{
	dmaChannel[ch]


}

void DMA_init(void)
{
   DMACFG0.SRCH=0xDF;			
   DMACFG0.SRCL=0xD9;		//Config Address
   DMACFG0.DESTH=0XFF;//((unsigned short)recbuff)>>8;
   DMACFG0.DESTL=((unsigned short)recbuff)&0xff;
   DMACFG0.VLEN=0X80;
   DMACFG0.LEN=0xff;
   DMACFG0.MODE=0x13;
   DMACFG0.STEP=0x10;
   
   DMACFG1.SRCH=0XFF;//((unsigned short)sendbuff)>>8;
   DMACFG1.SRCL=((unsigned short)sendbuff)&0xff;
   DMACFG1.DESTH=0xDF;
   DMACFG1.DESTL=0xD9;
   DMACFG1.VLEN=0X20;
   DMACFG1.LEN=0xff;
   DMACFG1.MODE=0x13;
   DMACFG1.STEP=0x40;   


   DMA0CFGH=0XFF;//(((unsigned short)(&DMACFG0))>>8);
   DMA0CFGL=(((unsigned short)(&DMACFG0))&0xff);
   
   DMA1CFGH=0XFF;//(((unsigned short)(&DMACFG1))>>8);
   DMA1CFGL=(((unsigned short)(&DMACFG1))&0xff);

}
//-----------------------------------------------------------------------------
// dma_IRQ
//
//  Description:
//      This interrupt routine is run each time the DMA generates an interrupt.
//      According to the interrupt flag of the DMA channel, the corresponding
//      callBackFunction is called. The interrupt flags are cleared.
//-----------------------------------------------------------------------------
#pragma vector=DMA_VECTOR
__interrupt void dma_IRQ (void){

   BYTE clearedFlags = 0x00;
   
   INT_GLOBAL_ENABLE(INT_OFF);   

   // Handle each channel
   if ((DMAIRQ & DMA_CHANNEL_0) && dmaTable[0].callBackFunction) { clearedFlags |= DMA_CHANNEL_0; dmaTable[0].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_1) && dmaTable[1].callBackFunction) { clearedFlags |= DMA_CHANNEL_1; dmaTable[1].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_2) && dmaTable[2].callBackFunction) { clearedFlags |= DMA_CHANNEL_2; dmaTable[2].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_3) && dmaTable[3].callBackFunction) { clearedFlags |= DMA_CHANNEL_3; dmaTable[3].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_4) && dmaTable[4].callBackFunction) { clearedFlags |= DMA_CHANNEL_4; dmaTable[4].callBackFunction(); }

   // Clear the flags
   INT_SETFLAG(INUM_DMA, INT_CLR);
   DMAIRQ = ~clearedFlags;

   INT_GLOBAL_ENABLE(INT_ON);
}
