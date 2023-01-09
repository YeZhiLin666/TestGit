/*****************************************************************************
// �����ļ�                   :MCUCommunication.h     ��Ҫ����MCU��UDPͨ��
// �ļ�����                   :ΪMCU board����
// ��д����(MM.DD.YYYY)       :04.26.2016
*****************************************************************************/

#ifndef _MCUCOMMUNICATION_H_
#define _MCUCOMMUNICATION_H_

#define MCU_PORT_M	         (7878)
#define MCU_PORT_S	         (7879)


#define MCU_MAX_WAITBUFFER_SIZE	         (100)   //  (64*1200*12=921.6K) 1200
#define MCU_MAX_SENDWINDOW_SIZE	         1//(10)      N
#define MCU_MAX_RESENDQUEUE_SIZE	     1//(MCU_MAX_SENDWINDOW_SIZE)
#define MCU_MAX_CONFIRMBUFFER_SIZE	     1//(MCU_MAX_SENDWINDOW_SIZE)
#define MCU_MAX_CMD_WAITBUFFER_SIZE      (80)    // 12 Length+4����ַ�'ZZZZ'
#define MCU_MAX_CMD_BUFFER_SIZE          (800)//(MCU_MAX_CMD_ITEM_SIZE*MCU_MAX_CMD_WAITBUFFER_SIZE)   // 30*12=360  Item*Length  


#define MCU_MAX_BUFFERTXINDEX_SIZE       8000

#define MCU_MAX_NUMBER                   64 //20 //UDP_NUMSOCKS
#define MCU_MAX_RECEIVEBUFFER_SIZE       (MCU_MAX_SENDWINDOW_SIZE+10)      // N+10(30*12)  ͬʱ������ݰ�����ѯ����ȷ�ϰ�  (64*11*360=253.44K)


//�����4M��������MCU�������й���
//1�����ݽ��������Ķ��л������Ի�����������ݵ㡣
#define  MCU_MAX_MCU_TOTAL   10
#define  MCU_MAX_WINDOW     10
#define  MCU_MAX_SEND_PACKBUFFER             MCU_MAX_WINDOW * 7  //  ��670��������1M�ռ�
#define  MCU_MAX_RECEIVE_PACKBUFFER          MCU_MAX_WINDOW * 6



#define  MCU_LIMITSENDSIZE               (1) //(10) 
#define  MCU_MAX_OUTTIME_REQUEST         (4000)
#define  MCU_MAX_OUTTIME_CONNECT         (100000)
#define  MCU_MAX_LIMITSEND_RECEIVEPACK       (1) //(3) 

#define MAX_CHARMGE_SIZE             (162)   //  60*1472/(62+360+120)=60*1472/542=162.95
#define MAX_CHAR_SIZE                (60*1472) // 60��  MAX_PACKLIMIT_SIZE_UDP ��󵥰��ֽ���UDP�������1514�ֽڣ��û�����1472�ֽ�

#define MCU_MAX_SEQUENCE_NUMBER      (55555)
#define MCU_MAX_SEND_OUTTIME1        (1000)// 100ms
#define MCU_MAX_SEND_CONFIRM1_NORMAL        (60) // 6ms
#define MCU_MAX_SEND_CONFIRM1_RAPIDLY_COMMUNICATE      (10)  // 1ms
#define MCU_MAX_SEND_ASK             (60) // 6ms*60 ʵ��6ms*34�δ�ԼΪ264ms
#define MCU_MAX_PRODUCE_PACKTIME     (40) // 4ms

#define MAX_RESENDPACK_NUM           (10)//(200)  // 50*200=10s   MCU_MAX_SEND_OUTTIME1*MAX_RESENDPACK_NUM

#define UDP_AUX_LIMIT_RATE           50 //(150) //  (1) // ����Limit��Ӧ������  N*20ms
#define UDP_WATCHDOGRATE_RATE        (3)   //  (1) // ���Ź���Ӧ������  N*20ms
#define UDP_PARALLEL_WATCHDOGRATE    (UDP_WATCHDOGRATE_RATE)   //  (1) // �������Ź���Ӧ������  N*20ms
#define UDP_CANBMS_RATE              (20)  //  (1) // ����CANBMS��Ӧ������  N*10ms


#define MAX_PACKLIMIT_SIZE_UDP                    (1472)  //  ��󵥰��ֽ���UDP�������1514�ֽڣ��û�����1472�ֽ�
#define MCU_MAX_CAN_CMD_DATA_SIZE      (MCU_MAX_CMD_WAITBUFFER_SIZE)    // 12 CAN Data Length

#define MCU_MAX_RECONNECT_TIME         (50000)  // 5s
#define MCU_MAX_RECONNECT_NUM          (5)      // 5s*5=25s
#define MCU_MAX_DIELOCKNUM             (5)      // 5s*5*5=2����

#define MCU_MAX_SENDCHARFAIL_NUM       (50)    // �����ֽ�ʧ��������

/*
typedef enum
{
    NONECONFIRM,
    GROUPCONFIRM,
	SINGLECONFIRM,
} TE_GET_MESSAGETYPE;*/

typedef enum
{
    ERROR_NONE,
    ERROR_CONNECT_LOST,
    ERROR_CHECK_FAIL,
    ERROR_STOPSCH,
    ERROR_QUEUE_FAIL,
    ERROR_TRANSFER_FAIL,
    ERROR_RECEIVE_FAIL,
    ERROR_OTHER,
} TE_MCU_ERROR_MESSAGETYPE;

typedef enum
{
    UDP_NONE,
    UDP_QUEUE_SENDWINDOW_FULL,
    UDP_QUEUE_UNIT_FULL,
    UDP_TRANSFER_BUFFER_FULL,
    UDP_TRANSFER_OFF_LINE,
    UDP_TRANSFER_GREAT_RESEND,
    UDP_RECEIVE_BUFFER_FULL,
    UDP_RECEIVE_UNKNOW_UNIT,
} TE_MCU_ERROR_CODE;

typedef enum
{
    UDP_MAP_NONE,
    UDP_MAP_AUX = 0X01,
    UDP_MAP_IV_PARALLEL = 0X02,
    UDP_MAP_CAN_BMS = 0X04,
} TE_MCU_MAP;

enum PACKET_STATUS
{
    PACKET_STATUS_CONNECT = 0X00,
    PACKET_STATUS_ASK = 0X01,
    PACKET_STATUS_CONFIRM = 0X02,
    PACKET_STATUS_DATA = 0X03,
};

enum SLICING_STATUS
{
    SLICING_STATUS_IDLE = 0X00,
    SLICING_STATUS_NUM = 0X01,
    SLICING_STATUS_TIME = 0X02,
};


typedef struct
{
    unsigned char   uc_StatusFlag:4;            // ״̬��־  ��λ
    unsigned char   uc_NoneFlag:3;              // Ԥ���ձ�־
    unsigned char   uc_IfFirstFlag:1;           // �׷�/�ط���־��־  ��λ (0X83)
    unsigned char   uc_SourceID;                // ԴID
    unsigned short int  uw16_SeqNum;            // �׷������У���������SN��
    unsigned short int  uw16_StarNum;           // ȷ�ϱ��ģ����SN��
    unsigned short int  uw16_EndNum;            // ȷ�ϱ��ģ��յ�SN��
    unsigned short int  uw16_ConnectNum;        // ���Ӻ�
    unsigned short int  uw16_PackItem;          // ����ѹ������ 30*12
    unsigned short int  uw16_PackLength;        // ���ݳ���  //  14�ֽ� ��λ
} St_MCU_WindowMessage;

#define  MCU_UDP_SN_PACKHEAD	     (8+sizeof(St_MCU_WindowMessage))

typedef struct
{
    unsigned char       uc_DestID;              // Ŀ��ID
    unsigned char       uc_IfSaveFlag;          // ���������Ƿ��Ѵ�����־
    //unsigned char       uc_VitalFlag;           // ��Ҫ���ݱ�־
    unsigned short int  uw16_PackLength;         // �������ݳ���
    uint8 	            m_UDPWaitBuffer[MCU_MAX_CMD_WAITBUFFER_SIZE];  // �������Ļ�����
} St_MCU_Data;


typedef struct
{
    unsigned char   uc_IfSaveFlag;              // ���������Ƿ��Ѵ�����־
    //unsigned char   uc_SourceID;                // ԴID
    unsigned char   uc_DestID;                  // Ŀ��ID
    unsigned short  uw16_ReSendCount;             // ���ط�����������
    //unsigned char   uc_IfSendFlag;              // ���������Ƿ��ܷ��ͱ�־
    //unsigned char   uc_VitalFlag;               // ��Ҫ���ݱ�־:�ɶ�/���ɶ�
    //unsigned short int  uw16_SeqNum;            // �����к�
    unsigned char    m_btNone_11[8];              // Ԥ��  BBBBBBBB
    St_MCU_WindowMessage    st_PackMess;              // ���д���ͷ��Ϣ    //  12�ֽ�
    uint8 			m_Data[MCU_MAX_CMD_BUFFER_SIZE];  // ���ݱ��Ļ�����
} St_MCU_InfoPack;

typedef struct
{
    unsigned char    m_btNone_11[8];              // Ԥ��  BBBBBBBB
    St_MCU_WindowMessage    st_PackMess;              // ���д���ͷ��Ϣ    //  12�ֽ�
} St_MCU_Head;

typedef struct
{
    unsigned char   uc_IfSaveFlag;              // ���������Ƿ��Ѵ�����־
    //unsigned char   uc_VitalFlag;               // ��Ҫ���ݱ�־
    unsigned char   uc_MCU_Seat;                // MCU��ȡλ��
    unsigned short int  uw16_SeqNum;            // �����к�
    unsigned short int  uw16_PackLength;         // �������ݳ���
    unsigned short int  uw16_PackAddress;       // ����������ַָ��
} St_MCU_MQueue;


// typedef struct
// {
//     //unsigned char   uc_Total;  // ������
//     St_MCU_MQueue     st_Address;     // ���������Ѵ����־
// } St_MCU_MQueue_Resend;

typedef struct
{
    //unsigned char   uc_Total;  // ������
    //unsigned short int  uw16_StarSeqNum;  // ��ʼ�����к�
    //unsigned short int  uw16_EndSeqNum;   // ���������к�
    St_MCU_MQueue       st_Address;       // ���������Ѵ����־
} St_MCU_MQueue_Window;

typedef struct
{
    //unsigned short int  uw16_PackLength;        // �������ݳ���
    unsigned short int  uw16_PackAddress;       // ����������ַָ��
} St_MCU_WAIT_MQueue;

typedef struct
{
    unsigned char         uc_DestID;         // Ŀ���
    //unsigned short int    uw16_DestPort;       // Ŀ��˿�
    unsigned short int    uw16_DLength;      // ���ݳ���
    //uint32                uw32_ByteLength;   // �ۼ��ֽڳ���
} St_MCU_CharMGE;

typedef struct
{
    unsigned char   uc_Address;        // �����±��ַ
    short int  w16_D_IP;               // Ŀ��IP
} St_NewTable;

typedef   CIRCULAR_QUEUE(St_MCU_WAIT_MQueue, MCU_MAX_WAITBUFFER_SIZE)         WaitQueue2;       // �ȴ����и�������,�൱��ָ��
typedef   CIRCULAR_QUEUE(St_MCU_MQueue_Window, MCU_MAX_CONFIRMBUFFER_SIZE)    ConfirmQueue2;    // ȷ�϶��и�������,�൱��ָ��

typedef   CIRCULAR_QUEUE(St_MCU_MQueue_Window, MCU_MAX_SENDWINDOW_SIZE)       SendWindow2;      // ���ʹ��ڶ��и�������,�൱��ָ��
typedef   CIRCULAR_QUEUE(St_MCU_MQueue_Window, MCU_MAX_RESENDQUEUE_SIZE)      ReSend2;          // �ط��Ͷ��и�������,�൱��ָ��

typedef   CIRCULAR_QUEUE(St_MCU_WAIT_MQueue, MCU_MAX_RECEIVEBUFFER_SIZE)    ReceiveWindow2;      // ���մ��ڶ��и�������,�൱��ָ��

typedef   CIRCULAR_QUEUE(uint8, MAX_CHAR_SIZE)                        CharSend1;         // �ֽڷ��Ͷ���
//typedef   CIRCULAR_QUEUE(uint32, MAX_CHARMGE_SIZE)                    CharMGE1;          // �ֽڷ��Ͷ��и�������,�൱��ָ��
typedef   CIRCULAR_QUEUE(St_MCU_CharMGE, MAX_CHARMGE_SIZE)            CharMGE2;          // �ֽڷ��Ͷ��и�������,�൱��ָ��


typedef struct
{
    unsigned char   uc_Active;               // MCU��Unit�Ż�Ծ
    unsigned char   uc_ConnectSuccess;       // ��MCU�������ӳɹ���־
    unsigned char   uc_ReConnectCounter;     // �������Ӽ�����
    unsigned char   uc_ID;                   // MCU��Unit��
    unsigned char   uc_Mapbit;                  // MCU��Map    bit0:AUX  bit1:IV Parallel bit2:CAN_BMS
    unsigned char   uc_SendTimeEN_On;        // ���ͳ�ʱ����
    unsigned char   uc_ReceiveTimeEN_On;     // ���ճ�ʱ����
    unsigned char   uc_ErrorType;            // ��������
    unsigned char   uc_ErrorCode;            // �������ʹ���
    unsigned char   uc_ErrorSeqNumCounter;   // ���յ����������������
    unsigned char   uc_SendPackCounter;       // ͬʱ���Ͱ�������
    unsigned long int   u32_Aux_IvRegister;            // ע��aux IVͨ����Ϣ
    unsigned long int   u32_Parallel_IvRegister;       // ע��parallel IVͨ����Ϣ
    unsigned long int   u32_BMS_IvRegister;            // ע��bms IVͨ����Ϣ

    unsigned short int  uw16_SeqNum;                    // IV���ͱ��ı��
    unsigned short int  uw16_StarNum;
    unsigned short int  uw16_EndNum;
    unsigned short int  uw16_ConnectNum;                // ���ӱ��

    unsigned short int  uw16_ConfirmSeqNum;             // Aux�ظ����ı��
    unsigned short int  uw16_ConfirmStarNum;
    unsigned short int  uw16_ConfirmEndNum;
    MsTime       m_WaitConfirmOutTime1;          // ���͵ȴ�ȷ�ϱ��ĳ�ʱ1
//    MsTime       m_WaitConfirmOutTime2;          // ���͵ȴ�ȷ�ϱ��ĳ�ʱ2
//    MsTime       m_WaitConfirmOutTime3;          // ���͵ȴ�ȷ�ϱ��ĳ�ʱ3
    MsTime       m_ConfirmConnectOutTime;        // ����ȷ�����ӱ��ĳ�ʱ
    MsTime       m_ReceiveWaitOutTime1;          // ���յȴ�ȷ�ϱ��ĳ�ʱ1
//    MsTime       m_ReceiveWaitOutTime2;          // ���յȴ�ȷ�ϱ��ĳ�ʱ2
//    MsTime       m_ReceiveWaitOutTime3;          // ���յȴ�ȷ�ϱ��ĳ�ʱ3
    unsigned short int    uw16_ReceiveWaitOutTime1_Counter;          // ���յȴ�ȷ�ϱ��ĳ�ʱ1������

} St_MCU_Unit;

class MultiMcuComm
{
public:
    static UInt16 Priority;
    static UInt16 TaskID;
    static void MyInit(void);
    static void DoWork(void);
//     static void ProduceMessageToWaitBuffer(uint8 uc_DestID, uint8 * content, unsigned int DataLen ,unsigned char VitalFlag);
//     static void SendMessageProcess(void);
//     static void ConfirmPack(UWord16 uw16_P);
//     static void CheckSinglePackQueue(unsigned char uc_Clear);
    static unsigned char Scan_IV_AUXMAP(void);
    static unsigned char Scan_IV_CANBMSMAP(void);
    static unsigned char Scan_IV_ParallelMAP(void);
    static void ReceiveData_ToBuffer (U8 *buf, U16 len, U8 *remip, U16 port);
    static unsigned char NET_Push_to_Send(const MP_MCU_MSG McuMsg);
    static void PackToNetLinkRoad(void);

    static void MCU_Connect_SendRequest(St_MCU_WindowMessage  *  WindowMessage);
//     static void MCU_Connect_Delete(unsigned char uc_DestID);
    static void ReceiveProcess(unsigned char uc_Index);
    static void TransferProcess(unsigned char uc_Index);
    static unsigned char PackToNetSendByte(unsigned char UnitID,unsigned char * Array,unsigned short int uw16_length);
    static unsigned char  NET_Rx_Msg(unsigned char * Data, unsigned short PackItem, unsigned int PackTotalLength);
    static unsigned char NET_Push_to_BroadCastSend(MP_MCU_MSG McuMsg);
    static void ReceiveData_ToBroadCastBuffer (U8 *buf, U16 len);
    static int UnregisterChannel(unsigned char uc_Index, unsigned char uc_ChannelNo ,unsigned char Map);
	static char Scan_EmptySeat(unsigned char UnitID,unsigned char IV_Ch,unsigned char Map);
	static void Build_NewTable(void);
	static void Scan_Clear(unsigned char Map);

private:
    static short int  MoveWaitBufferToSendBuffer(unsigned char  uc_Index, unsigned short int Length);
    static int PushingPackToWaitBuffer(uint8 uc_DestID, uint8 * content, unsigned int DataLen); // ,unsigned char VitalFlag);
    static void ErrorProcess(unsigned char uc_Index);
    static short int ReSendSingleMessageProcess(unsigned char uc_Index,UWord16 uw16_SN);
    static void MCU_ReconnectToMCU(unsigned char uc_Index);
//     static void ResendMessageProcess(void);
//     static void FlowControlProcess(void);
    static void AbsolveDieLockProcess(unsigned char uc_Index);
    static short int FindConfirmBuffer(unsigned char uc_Index);
    static int RegisterChannel(unsigned char uc_Index, unsigned char uc_ChannelNo ,unsigned char Map);

//    static void ReceiveProcess(unsigned char uc_Index);
//   static void TransferProcess(unsigned char uc_Index);
    static short int Seek_Unit(uint8 uc_DestID);
//     static short FindUnitIndex(unsigned char uc_UnitID);
    //static void MCU_SendConnectRequest(St_MCU_WindowMessage  *  WindowMessage);
    static void MCU_SendAsk(St_MCU_WindowMessage  *  WindowMessage);
    static void MCU_SendConfirm(St_MCU_WindowMessage  *  WindowMessage);
    static unsigned char MCU_ConfirmProcess(unsigned char uc_Index,St_MCU_WindowMessage  *  WindowMessage);
    static char SlicingProcess(void);
    static short int FindReceiveBuffer(unsigned char uc_Index);
    static unsigned char UpdataConfirmStarNum(unsigned char uc_Index,St_MCU_WindowMessage  *  Message);
    static void ProduceMessageToSendBuffer(unsigned char uc_Index,unsigned int DataLen);
    static void MCU_ClearConfirmBuffer(unsigned char uc_Index,unsigned char uc_Numb);
    static void MCU_ClearReceiveBuffer(unsigned char uc_Index,unsigned char uc_Numb);
    static short MCU_ConfirmBuff_FindMin(unsigned char uc_Index);
    static void MCU_ConfirmBuff_UpdataSeqNum(unsigned char uc_Index);
    static unsigned char NET_BroadCast_to_Send(const MP_MCU_MSG McuMsg);

public:

    /*
                   static  unsigned long int uw32_Scounter[MCU_MAX_NUMBER];
                   static  unsigned long int uw32_Rcounter[MCU_MAX_NUMBER];
                   static  unsigned short int uw16_ST1;
                   static  unsigned short int uw16_RT1;
                   static  unsigned short     s_d_SN[40];
                   static  unsigned long      s_d_Second[40];
                   static  unsigned long      s_d_Us100[40];
                   static  unsigned short     r_d_SN[40];
                   static  unsigned long      r_d_Second[40];
                   static  unsigned long      r_d_Us100[40];*/

    static unsigned char       uc_SlicingStatus;                                // MCU��������״̬
    static unsigned char       uc_SlicingEnable;                                  // MCU��������ʹ��
    static St_MCU_InfoPack     St_ConfirmBuffer[MCU_MAX_NUMBER][MCU_MAX_CONFIRMBUFFER_SIZE];   // ȷ�����ݱ��Ļ�����
    static St_MCU_Unit         St_Unit[MCU_MAX_NUMBER];           // MCU��Unit��
    static unsigned short int  uw16_CharFailCounter; // �ֽڷ���ʧ�ܼ�����
    static CharSend1           CharSend;         // �ֽڷ��Ͷ���
    static CharMGE2            CharMGE;          // �ֽڷ��Ͷ��и�������,�൱��ָ��
    static MsTime              m_SlicingOutTime;                               // MCU����ʱ�����Ƴ�ʱ
    static St_MCU_WindowMessage  WinMess;
// 	static uint8     * this_ReceiveBuff_Data;
    static unsigned char   uc_udp_BroadCastdata[MCU_MAX_CMD_WAITBUFFER_SIZE];
    static UWord32         uw32_MaxTime_Confirm;
private:
    static St_MCU_InfoPack     St_ReceiveBuffer[MCU_MAX_NUMBER][MCU_MAX_RECEIVEBUFFER_SIZE];     // ���ձ��Ļ�����
    static St_MCU_Data 	       St_WaitBuffer[MCU_MAX_NUMBER][MCU_MAX_WAITBUFFER_SIZE];       // �������Ļ�����

    static WaitQueue2         WaitQueue[MCU_MAX_NUMBER];            // �ȴ����и�������,�൱��ָ��
    static ConfirmQueue2      ConfirmQueue[MCU_MAX_NUMBER];         // �ط��Ͷ��и�������,�൱��ָ��
    static SendWindow2        SendWindowQueue[MCU_MAX_NUMBER];      // ���ʹ��ڶ��и�������,�൱��ָ��
    static ReSend2            ReSendQueue[MCU_MAX_NUMBER];          // �ط��Ͷ��и�������,�൱��ָ��
    static ReceiveWindow2     ReceiveWindowQueue[MCU_MAX_NUMBER];   // ���մ��ڶ��и�������,�൱��ָ��

//     static MsTime              m_PackProduceTime[MCU_MAX_NUMBER];              // ���ɱ��ļ��ʱ��
//     static unsigned short int  uw16_LimitSendWindowSize_m;     // ���Ʒ��ʹ��ڴ�С����
//     static unsigned short int  uw16_AddSendWindowSize_n;       // ���ӷ��ʹ��ڴ�С����
//     static unsigned short int  uw16_ConfirmTotal;
//	static unsigned char       uc_Scan_Index;

    static unsigned char      uc_udp_Temp[100];
    static unsigned char      uc_udp_data[MCU_MAX_CMD_WAITBUFFER_SIZE];

    static unsigned short int  uw16_AbsolveDieLockCounter[MCU_MAX_NUMBER];
    static uint8 	           uc_CmdBuffer_Temp[MAX_PACKLIMIT_SIZE_UDP];
    static St_NewTable         st_UnitNewTable[MCU_MAX_NUMBER];                   // �²�ѯ��
    static unsigned char       uc_UnitNewTable_Total;                             // �²�ѯ������
    static UWord32             uw32_Time_Connect;
};


#endif
