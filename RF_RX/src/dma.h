#ifndef __DMA_H__
#define	__DMA_H__
#include "mytypedef.h"
#include "ioCC1110.h"

#define DMA_NUMBER		2
#define DMA_CHANNEL_0  0x01
#define DMA_CHANNEL_1  0x02
#define DMA_CHANNEL_2  0x04
#define DMA_CHANNEL_3  0x08
#define DMA_CHANNEL_4  0x10

#define INT_ON   1
#define INT_OFF  0
#define INT_SET  1
#define INT_CLR  0

#define INT_GLOBAL_ENABLE(on) EA=(!!on)
#define DISABLE_ALL_INTERRUPTS() (IEN0 = IEN1 = IEN2 = 0x00)
struct 
{
    unsigned char SRCH;
    unsigned char SRCL;
    unsigned char DESTH;
    unsigned char DESTL;
    unsigned char VLEN;
    unsigned char LEN;
    unsigned char MODE;
    unsigned char STEP;
}DMACFG0,DMACFG1;
//for user struct
typedef struct {
    BYTE        assigned;
    FUNCTION*   callBackFunction;
} DMA_TABLE;
#pragma bitfields=reversed
typedef struct {
   BYTE SRCADDRH;
   BYTE SRCADDRL;
   BYTE DESTADDRH;
   BYTE DESTADDRL;
   BYTE VLEN      : 3;
   BYTE LENH      : 5;
   BYTE LENL      : 8;
   BYTE WORDSIZE  : 1;
   BYTE TMODE     : 2;
   BYTE TRIG      : 5;
   BYTE SRCINC    : 2;
   BYTE DESTINC   : 2;
   BYTE IRQMASK   : 1;
   BYTE M8        : 1;
   BYTE PRIORITY  : 2;
} DMA_DESC;
#pragma bitfields=default

__interrupt void dma_IRQ (void);		//

void txCallBack(void);
void rxCallBack(void);
#endif