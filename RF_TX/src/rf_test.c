#include "mytypedef.h"
#include "myio.h"
#include "cul.h"

#define RF_RETRY_TIMES  10
#define	ADDRESS_0	0x01
#define	ADDRESS_1	0x02
#define	RECEIVE_TIMEOUT	1200
extern void UartSendString(uchar *data,int len);
void receiveMode(void);
void contionuousMode(void);

uchar	myAddr;
uchar	remoteAddr;
BOOL	remoteAddressSet=FALSE;
BOOL	myAddressSet=FALSE;
INT32U	frequency;

void SingleMoreTDMA(char sell)
{
  if(sell == 0)
  {
	UartSendString("123" ,4);	
    myAddr = ADDRESS_1;
		remoteAddr = ADDRESS_0;
		radioInit(frequency, myAddr);
		INT_GLOBAL_ENABLE(INT_ON);
		contionuousMode();
	  }
	  else if(sell == 1)
	  {
		myAddr = ADDRESS_0;
		remoteAddr = ADDRESS_1;
		halWait(10);
		radioInit(frequency, myAddr);
		INT_GLOBAL_ENABLE(INT_ON);
		halWait(10);
		receiveMode();
          }
}


/******************
* for transferring packets 
*/
void contionuousMode(void)
{
#ifdef CC1110_TDMA_TX	
	BOOL	res;
	INT8U	count;
	BYTE*	receiveBuffer;
	BYTE	RXbuff_command[7];
	BYTE	length;
	INT8U	key=0;
	BYTE	sender;
	BYTE	sendBuffer[5];
	sendBuffer[0]=0x40;
	#ifdef CC1110_TDMA_TX1
		sendBuffer[2] = 0x01;
	#endif
	#ifdef CC1110_TDMA_TX2
	   sendBuffer[2] = 0x02;
	#endif
	  sendBuffer[4] = 0x0D;
         LED1=~LED1;  
         UartSendString("1232" ,4);	
	while(1)
	{
		if(KEY1==0)
		{
			while(KEY1==0);
			while(KEY1==0);
			key='1';
                      //  LED1=~LED1;
                        UartSendString("ccc" ,3);
		}
    
               
                
		while(key!=0)
		{
			/*res=radioReceive(&receiveBuffer,&length,RECEIVE_TIMEOUT,&sender);
			RXbuff_command[0]=*receiveBuffer++;
			RXbuff_command[1]=*receiveBuffer++;
			RXbuff_command[2]=*receiveBuffer++;
			sendBuffer[3]=key;
                  */
		#ifdef  CC1110_TDMA_TX1
			//if((res==TRUE)&&(RXbuff_command[0]=='1')&&(RXbuff_command[1]=='B'))
		#endif
		#ifdef CC1110_TDMA_TX2
			if((res==TRUE)&&(RXbuff_command[0]=='2')&&(RXbuff_command[1]=='B'))
		#endif
			{
				for(int i=0;i<7;i++)
					RXbuff_command[i]=0;
				LED1=~LED1;
				res=radioSend(sendBuffer,sizeof(sendBuffer),remoteAddr,DO_ACK);
                                
				if(res==TRUE)
				{
					key=0;
                                        UartSendString("SSSS" ,4);
					if(++count>1)
					{	if(count>=RF_RETRY_TIMES)
							count=2;
                                                key=0;
						break;
					}
				}else
				{
					  UartSendString("FFFF" ,4);
                                    count++;
					if(count>=RF_RETRY_TIMES)
					{
						count=2;
                                                key=0;
						break;
					}
				}	//end if  res=false
			}		//endif tx
		}			//endif while key
                
	}				//endif while 1
#endif
}

uchar number1=0,number2=0,number[4];
void receiveMode(void)
{
	BYTE *receiveBuffer;
	BYTE RXbuffer[5];
	BYTE sendBuffer[3]="1B";
	BYTE length;
	BYTE res;
	BYTE sender;
	uchar type=0;
	uchar ReceiveCount=5;
	while(1)
	{
		if(type>=2)
			type=1;
		else
			type++;
		ReceiveCount=3;
		while(ReceiveCount--)
		{
			sendBuffer[0]=0x40+type;	//		
			res=radioSend(sendBuffer,sizeof(sendBuffer),remoteAddr,DO_ACK);
			if(res==TRUE)
			{
				res=radioReceive(&receiveBuffer,&length,30,&sender);
				if(res==TRUE)
				{
					LED2=~LED2;
					RXbuffer[0]=*receiveBuffer++;
					RXbuffer[1]=*receiveBuffer++;
					RXbuffer[2]=*receiveBuffer++;
					RXbuffer[3]=*receiveBuffer++;
					UartSendString(RXbuffer,sizeof(RXbuffer));
					break;
				}else
				{
					LED2=LED_OFF;
				}		// receive true
			}	//Send success
		}	//end while 
	} //end while 1
}


	