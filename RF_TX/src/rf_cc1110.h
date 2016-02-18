
#ifndef _RF_CC1110_H_
#define _RF_CC1110_H_

#include "mytypedef.h"
#include "ioCC1110.h"

#define SFSTXON()     do{RFST = 0x00;}while(0)
#define SCAL()        do{RFST = 0x01;}while(0)
#define SRX()         do{RFST = 0x02;}while(0)
#define STX()         do{RFST = 0x03;}while(0)
#define SIDLE()       do{RFST = 0x04;}while(0)
#define SAFC()        do{RFST = 0x05;}while(0)
#define SNOP()        do{RFST = 0xFF;}while(0)















#endif //_RF_CC1110_H_


