//spp.h
/******************************************************************************
*******************      Simple Packet Protocol (SPP)      ********************
******************************************************************************/
#define SPP_MAX_PAYLOAD_LENGTH          122 // This length may be extended up to 250
#define SPP_HEADER_AND_FOOTER_LENGTH    3
#define SPP_FOOTER_LENGTH               0
#define SPP_ACK_LENGTH                  0

typedef struct{
    BYTE payloadLength;
    BYTE destAddress;
    BYTE srcAddress;
    BYTE flags;
    BYTE payload[SPP_MAX_PAYLOAD_LENGTH + SPP_FOOTER_LENGTH];
}SPP_STRUCT;

#define BROADCAST_ADDRESS               0
#define WAIT_UNTIL_RECEIVE              0

#define SEQUENCE_BIT                    0x80
#define RETRANSMISSION                  0x04
#define ACK                             0x02
#define DO_ACK                          0x01
#define DO_NOT_ACK                      0x00


#define ACK_TIMEOUT                     15
#define ACK_RETRIES                     3

// TX status flags
#define TX_IN_PROGRESS                  0x80
#define TX_SUCCESSFUL                   0x40
#define DEST_UNREACHABLE                0x20
#define TX_IDLE                         0x00

// RX status flags
#define RX_IN_PROGRESS                  0x80
#define PACKET_RECEIVED                 0x40
#define RX_WAIT                         0x20
#define RX_COMPLETE                     0x10
#define RX_IDLE                         0x00

// sppSend return values
#define CHANNEL_BUSY                    0x04
#define TOO_LONG                        0x08

