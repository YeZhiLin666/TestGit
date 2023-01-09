#pragma once


#define DATA_SEND     1
#define DATA_SAVE     0


#define  MAX_BUF_LENGTH     20
#define  MAX_DATA_LENGTH    64 //256
#define  MAX_RESEND_BUF_LENGTH 64
#define  USE_COMPLETE_DMA_CONFIG   0  // complete config 

typedef struct
{
    unsigned char m_Length;
    unsigned char m_RxBuf[MAX_UART_RX_LENGTH]; //MAX_DATA_LENGTH
} St_OneDMABuf;
typedef struct
{
    unsigned char m_front;
    unsigned char m_rear;
    unsigned char m_length;
    St_OneDMABuf m_Buf[MAX_BUF_LENGTH];
} St_QueueBuf;

typedef struct
{
    unsigned char m_front;
    unsigned char m_rear;
    unsigned char m_length;
    St_UartCmdBuf m_Buf[MAX_RESEND_BUF_LENGTH];
} St_RESENDBuf;

class UartParse
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
    static St_QueueBuf  m_UartRxQueue;
    static St_RESENDBuf  ReSendUartMsg; //????

public:
    static void MyInit(void);
    static void DoWork(void);
    static void Parse(void);
    static void SendCmd(St_UartCmdBuf * Uart_Cmd);
    static void Parse_UnsafeInfo(const St_Uart_ReportUnsafe * UnsafeInfo);
    static void Parse_ReportData(const St_Uart_GetADCVoltage * Data);
    static void Parse_ReportCalibPara( St_Uart_ReportCalibPara *Para);
    static void AckCheck(St_Uart_ReportAck *Data);
    static void ResendMsg();
    static void DelResendMsg( int index );
    static void PushToResendMsg(St_UartCmdBuf * Uart_Cmd);
};
