#pragma once


#define  M0MAX_UART_QUENUE_SIZE  10
#define  M0MAX_UART_RX_LENGTH    24

#define  MAX_BUF_LENGTH     20
#define  MAX_DATA_LENGTH    64 //256
#define  USE_COMPLETE_DMA_CONFIG   0  // complete config 

typedef struct
{
    unsigned char m_Length;
    unsigned char m_RxBuf[MAX_DATA_LENGTH];
} St_OneDMABuf;
typedef struct
{
    unsigned char m_front;
    unsigned char m_rear;
    unsigned char m_length;
    St_OneDMABuf m_Buf[MAX_BUF_LENGTH];
} St_QueueBuf;



class AuxUartParse
{
public:
    static  UInt16        Priority ;
    static  UInt16        TaskID ;
    static void Init(void);
    static void DoWork(void);
private:
    static St_QueueBuf  	  m_UartRxQueue;

public:
// 	static void ParseOneCmd(unsigned char idx);
    static void DoUartParse(void);
    static void Send_UartCmd(St_UartCmdBuf * Uart_Cmd);
    static unsigned char * UartChecksumPro( unsigned short* DataLen ,unsigned char* pDataBuf );
    static unsigned short MyCheckSum(unsigned short *buffer, unsigned short size);
};
