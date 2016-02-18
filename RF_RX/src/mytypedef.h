/*
================================================================================
File Name    : MyTypeDef.H
Description  : universal Data type definations
Ahthor       : LiYong
================================================================================
*/
#ifndef _MYTYPEDEF_H_
#define _MYTYPEDEF_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "mytypedef.h"
/*Data type definations  */
typedef unsigned char  INT8U;
typedef signed   char  INT8S;
typedef unsigned int   INT16U;
typedef signed   int   INT16S;
typedef unsigned long  INT32U;
typedef signed   long  INT32S;
typedef unsigned char	BYTE;
typedef void FUNCTION(void);
typedef enum
{
	BOOL_FALSE = 0,
	BOOL_TRUE  = 1
}BOOLEAN;

#ifndef TRUE
#define TRUE   ( 1 )
#endif
#ifndef FALSE
#define FALSE  ( 0 )
#endif

/*Bit field operations*/
#define SetBit( Byte, Bit )  ( Byte ) |= ( 1<<( Bit ) )
#define ClrBit( Byte, Bit )  ( Byte ) &= ~( 1<<( Bit ) )
#define GetBit( Byte, Bit )  ( ( Byte ) & ( 1<<( Bit ) ) )
#define ComBit( Byte, Bit )  ( Bytes ) ^= ( 1<<( Bit ) )
#define SetBits( Byte, Bits ) ( Byte ) |= ( Bits )
#define ClrBits( Byte, Bits ) ( Byte ) &= ~( Bits )
#define GetBits( Byte, Bits ) ( ( Byte ) & ( Bits ) )
#define ComBits( Byte, Bits ) ( Byte ) ^= ( Bits )

/*Bit map for fast calculation*/
#define BitMap( x )   ( 1<<( x ) )
/*Get MAX or MIN value*/
#define GetMax( x1, x2 ) ( ( x1 ) > ( x2 ) ? ( x1 ) : ( x2 ) )
#define GetMin( x1, x2 ) ( ( x1 ) > ( x2 ) ? ( x2 ) : ( x1 ) )


// Global interrupt enables
#define INT_ON   1
#define INT_OFF  0
#define INT_SET  1
#define INT_CLR  0
#define INT_GLOBAL_ENABLE(on) EA=(!!on)
#define DISABLE_ALL_INTERRUPTS() (IEN0 = IEN1 = IEN2 = 0x00)


#define INT_GETFLAG(inum) (                       \
   (inum==INUM_RFTXRX)      ? RFTXRXIF          : \
   (inum==INUM_ADC)         ? ADCIF             : \
   (inum==INUM_URX0)        ? URX0IF            : \
   (inum==INUM_URX1)        ? URX1IF            : \
   (inum==INUM_ENC)         ? ENCIF_0           : \
   (inum==INUM_ST)          ? STIF              : \
   (inum==INUM_P2INT)       ? P2IF              : \
   (inum==INUM_UTX0)        ? UTX0IF            : \
   (inum==INUM_DMA)         ? DMAIF             : \
   (inum==INUM_T1)          ? T1IF              : \
   (inum==INUM_T2)          ? T2IF              : \
   (inum==INUM_T3)          ? T3IF              : \
   (inum==INUM_T4)          ? T4IF              : \
   (inum==INUM_P0INT)       ? P0IF              : \
   (inum==INUM_UTX1)        ? UTX1IF            : \
   (inum==INUM_P1INT)       ? P1IF              : \
   (inum==INUM_RF)          ? S1CON &= ~0x03    : \
   (inum==INUM_WDT)         ? WDTIF             : \
   0                                              \
)



// Macro used to set or clear certain interrupt flags.
// Example usage:
//   INT_SETFLAG(INUM_URX0, INT_SET;
//   INT_SETFLAG(INUM_T3, INT_CLR);
#define INT_SETFLAG(inum, f)                     \
   do {                                          \
      if      (inum==INUM_RFTXRX){ RFTXRXIF = f; } \
      else if (inum==INUM_ADC)   { ADCIF  = f; } \
      else if (inum==INUM_URX0)  { URX0IF = f; } \
      else if (inum==INUM_URX1)  { URX1IF = f; } \
      else if (inum==INUM_ENC)   { ENCIF_1 = ENCIF_0 = f; } \
      else if (inum==INUM_ST)    { STIF  = f;  } \
      else if (inum==INUM_P2INT) { P2IF  = f;  } \
      else if (inum==INUM_UTX0)  { UTX0IF= f;  } \
      else if (inum==INUM_DMA)   { DMAIF = f;  } \
      else if (inum==INUM_T1)    { T1IF  = f;  } \
      else if (inum==INUM_T2)    { T2IF  = f;  } \
      else if (inum==INUM_T3)    { T3IF  = f;  } \
      else if (inum==INUM_T4)    { T4IF  = f;  } \
      else if (inum==INUM_P0INT) { P0IF  = f;  } \
      else if (inum==INUM_UTX1)  { UTX1IF= f;  } \
      else if (inum==INUM_P1INT) { P1IF  = f;  } \
      else if (inum==INUM_RF)    { (f) ? (S1CON |= 0x03) : (S1CON &= ~0x03); } \
      else if (inum==INUM_WDT)   { WDTIF = f;  } \
   } while (0)


#endif//_MYTYPEDEF_H_
/*
================================================================================
====================================End of file=================================
================================================================================
*/
