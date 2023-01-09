
#define  MAX_BUF_LENGTH     20
#define  MAX_DATA_LENGTH    64 //256

typedef struct
{
    unsigned char m_front;
    unsigned char m_rear;
    unsigned char m_length;
    unsigned char m_Buf[MAX_BUF_LENGTH][MAX_DATA_LENGTH];
} St_DWIN_QueueBuf;

class DWIN_UartParse
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
    static St_DWIN_QueueBuf m_DWIN_RxQueue;
//	static unsigned short DataVarAddress;
//	static unsigned short AddressInc;
    static unsigned short CRC_Check( unsigned char *pDataBuf,unsigned char Datalength);
    static void DoDWINParse(void);
    static void DWIN_SendCmd(unsigned char  * Uart_Cmd,unsigned char length);
    static void DWIN_ACK(unsigned short address, unsigned char length);
    static void UpdateValue(unsigned short address, long data);
    static void MTCReportErr(unsigned short address, unsigned short data);
    static void MTCSwitchToRemote();
    static void MTCKeepRemote();
    static void DeleteRowData(unsigned short dataNo);
    static void Init(void);
    static void DoWork(void);
    static void SwitchModeReset(unsigned char AuxChanNo);
    static void MITSwitchRemote(unsigned char AuxChanNo);

};
