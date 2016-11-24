#define RF_FREQ_433MHZ  433000
#define RF_FREQ_868MHZ  868000
#define RF_FREQ_902MHZ  902000



#define MAX_NUM     5
#define RETRY_TIMES   2
#define WAIT_TIMES    1
#define MSG_TIMEOUT     10
// msg type 
#define BROADCAST   0xFF

#define MSG_TIME      0x01
#define MSG_ACCESS      0x02
#define MSG_SUCCESS      0x03

#define MSG_BROADCAST   0xFF
#define MSG_OK		0x08


#define UART_MSG_ORDER    1
#define UART_MSG_ACCESS   2
#define UART_MSG_SUCCESS  3
#define UART_MSG_FAIL     4
#define AP_NO_RE_FOUND          0x05

#define AP_RFMSG_TIME_RE	0x65	//��վ���м�������ʱ��Ƭ(REID=1��
#define AP_RFMSG_ACCESS_RE	0x66	//��վ�յ��м������ܳɹ���Ϣ
#define AP_RFMSG_SUCCESS_RE	0x67	//��վ�յ��м����ɹ�������Ϣ
#define AP_RFMSG_FAIL_RE	0x68	//��վ�յ��м���������Ӧʧ����Ϣ
#define AP_RFMSG_TIMEOUT	0x69	//��վ��ʱ��û�յ��м���(�ڲ�״̬��


#define UART_MSG_TEST              0xEE         //����ģʽ          

#define UART_MSG_STATUS            0xEF        //�õ���վ��ǰ״̬�� busy��wait�� free�� 
#define UART_MSG_FREQ_STATUS       0xF0     //�õ�ͨ��״̬    
#define UART_MSG_FREQ_CHANGE       0xF1    //�ı�ͨ��״̬
#define UART_CMD_SIZE   24
#define RE_CMD_SIZE     24
#define BUFFER_SIZE   17
//machine ID
#define LED_TX    P1_0
#define LED_RX    P1_1
#define LED_ON    0
#define LED_OFF       1

#define KEY1    P1_2
#define KEY2    P1_3
//External functions
/*** Define Data iD */

#define CHAR_NUM        4*2
#define RF_NUM          0
#define RF_MSG          RF_NUM+2
#define RF_REID        RF_MSG+1
#define RF_MACID        RF_REID+1
#define RF_TABLE        RF_MACID+1
#define RF_ORDER        RF_TABLE+CHAR_NUM
#define RF_SUM          RF_ORDER+4
#define RF_IDLIST       RF_SUM+1

#define RF_TEST