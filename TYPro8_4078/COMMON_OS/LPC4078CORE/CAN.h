/*****************************************************************************
// 程序文件      :CAN.h     主要用于CAN硬件驱动。
// 文件描述      :为LPC4078 MCU board服务
// 编写日期      :2012.07.13
*****************************************************************************/

#ifndef __CAN_H_
#define __CAN_H_


//----CAN----------------------------------------------------------------------------
//CAN部分，保留，一般不用修改
#define CAN_BUF_INDEX_LEN		5

#define   MAX_CAN_FRAME_LEN           8
//#define   CAN_EXT_FRAME               1
#define  TXBUFGROUP                    4
#define AllowIntNum              10    //允许中断数
#define CAN_BYTE_MAX_LEN     8
//----CAN----------------------------------------------------------------------------

// CAN接收超时长度，单位毫秒
#define CAN_TIMEOUT				5000 //	500

#define ACCEPTANCE_FILTER_ENABLED	1

#define CAN_MEM_BASE		0x40038000

#define MAX_PORTS	2		/* Number of CAN port on the chip */

/* BRP+1 = Fpclk/(CANBitRate * QUANTAValue)
QUANTAValue = 1 + (Tseg1+1) + (Tseg2+1)
QUANTA value varies based on the Fpclk and sample point
e.g. (1) sample point is 87.5%, Fpclk is 48Mhz
the QUANTA should be 16
(2) sample point is 90%, Fpclk is 12.5Mhz
the QUANTA should be 10
Fpclk = Fclk /APBDIV
or
BitRate = Fcclk/(APBDIV * (BRP+1) * ((Tseg1+1)+(Tseg2+1)+1))
*/
/* Here are some popular bit timing settings for LPC2468, google on "SJA1000"
CAN bit timing, the same IP used inside LPC2000 CAN controller. There are several
bit timing calculators on the internet.
http://www.port.de/engl/canprod/sv_req_form.html
*/


/* When Fpclk is 60Mhz  respectively.
Common CAN bit rates for 60Mhz clock frequency */
//when Fpclk is 60Mhz, sample point is 83.3%
#define CANBAUD125k60M		0x0085003B          //60M 波特率计算器TSEG1/2   5,0    
#define CANBAUD250k60M	   0x008D000E          //60M 波特率计算器TSEG1/2   3,0   
#define CANBAUD500k60M	   0x008C0007        //60M 波特率计算器TSEG1/2   12,0   
#define CANBAUD1M60M	      0x008c0003        //60M 波特率计算器TSEG1/2   3,0  

#define CANBAUD125k18M		0x0085003B          //60M 波特率计算器TSEG1/2   5,0    
#define CANBAUD250k18M		0x00830027          //60M 波特率计算器TSEG1/2   3,0   
#define CANBAUD500k18M	    0x008C0007        //60M 波特率计算器TSEG1/2   12,0  
#define CANBAUD1M18M	    0x00830006        //60M 波特率计算器TSEG1/2   3,0  

enum
{
    CANBAUD125k  = 0,
    CANBAUD250k = 1,
    CANBAUD500k = 2,
    CANBAUD1M = 3,
};
//注意使用周立功CAN盒时，对应设置表如下：
//   CAN波特率			 R0          R1
//     125K               0X03      0X1C
//     250K               0X01      0X1C
//     500K               0X00      0X1C
//      1M                0X00      0X14


enum
{
    CAN1 = 0,
    CAN2 = 1,
    RUDPNET =2,
};

// enum COMMUN_NETWORT
// {
// 	MCU_TO_MCU_CAN = 10,
//   MCU_TO_MCU_NET = 20,
// };

enum
{
    RX_EMPTY = 0,
    RX_FULL = 1,
    RX_OK = 2,
    RX_ERROR ,
    RX_HALFFULL ,
    RX_ALMOSTFULL ,
};
enum
{
    TX_EMPTY = 0,
    TX_FULL = 1,
    TX_HALFEMPTY =2,
    TX_HALFFULL = 3,
    TX_ALMOSTFULL = 4,
};

//* Acceptance filter mode in AFMR register */
enum
{
    ACCF_ON = 0,
    ACCF_OFF = 1,
    ACCF_BYPASS = 2,     //旁路模式
    ACCF_FULLCAN = 4 ,   //全局滤波模式
};

typedef struct
{
    unsigned int ExtLow;
    unsigned int ExtHigh;
} CANFilterExtGrp;

//Interrupt Status Register
enum  CAN_BIT_ISR
{
    CANRI = 0x0001 ,   //0x01
    CANTI1 = 0x0002,
    CANEI =  0x0004,
    CANDOI = 0x0008,
    CANWUI = 0x0010,
    CANEPI = 0x0020,
    CANALI = 0x0040,
    CANBEI = 0x0080,
    CANIDI = 0x0100,
    CANTI2 = 0x0200,
    CANTI3 = 0x0400,

};

enum CAN_TX_RES
{
    CAN_TXOK = 0XA0,
    CAN_SENDING = 0XA1,
    CAN_TXFAILED = 0XA2,
    CAN_TXERR1 = 0XA3,     //发送错误
    CAN_TXBUF_ERR = 0XA4 ,
    CAN_BUS_ERROR = 0xA5,
};

enum CAN_GSR_REG
{
    GSR_BUFVALUE = 1,
    GSR_BUFDOS   = 2,
    GSR_ES = 0x40,
    GSR_BS = 0x80,
};
enum CAN_RX_RES
{
    CAN_RXOK = 1,
    CAN_RX_ERR = 2,
};

enum CAN_TBS
{
    TBS1 = 0x00000004,
    TBS2 = 0x00000400,
    TBS3 = 0x00040000,
};
enum  CAN_TCS
{
    TCS1 = 0x00000008,
    TCS2 = 0x00000800,
    TCS3 = 0x00080000,
};
enum
{
    CAN_PUSH_OK = 0,
    CAN_BACK_OK = 1,
    CAN_BUF_FULL = 2,
    CAN_PUSH_FAIL = 3,
    CAN_RESTORE_OK = 4,
    CAN_RESTORE_FAIL = 5,
};
enum  FRAMETYPE
{
    CAN_STD_FRAME = 0,
    CAN_EXT_FRAME = 1,
};


/* This number applies to all FULLCAN IDs, explicit STD IDs, group STD IDs,
explicit EXT IDs, and group EXT IDs. */
#define ACCF_IDEN_NUM			4

/* Identifiers for FULLCAN, EXP STD, GRP STD, EXP EXT, GRP EXT */
#define FULLCAN_ID				0x100
#define EXP_STD_ID				0x100
#define GRP_STD_ID				0x200
#define EXP_EXT_ID				0x100000
#define GRP_EXT_ID				0x200000

// Error status words:
//    -bits 0-7 are current errors
//    -bits 8-15 are corresponding latched errors
#define CAN_XMT_ERR             0x0101
#define CLR_CAN_XMT_ERR         0xFFFE
#define CAN_XMT_TOUT            0x0202
#define CLR_CAN_XMT_TOUT        0xFFFD
#define CAN_RCV_ERR             0x0404
#define CLR_CAN_RCV_ERR         0xFFFB
#define DSP_SYS_ERR             0x0808


/**************************************************************************
PUBLIC FUNCTIONS
***************************************************************************/

//<<End  CAN 封闭式函数>>  */

typedef union
{
    struct
    {
        unsigned int  PRIO:8;
        unsigned int  Rsvda:8;
        unsigned int  DLC:4;
        unsigned int  Rsvdb:10;
        unsigned int  RTR:1;
        unsigned int  FF:1;
    } BitFrame;
    unsigned int  Frame;
} UN_CAN_TFI;

typedef union
{
    struct
    {
        unsigned int  INDEX:10;
        unsigned int  BP:1;
        unsigned int  Rsvda:5;
        unsigned int  DLC:4;
        unsigned int  Rsvdb:10;
        unsigned int  RTR:1;
        unsigned int  FF:1;
    } BitFrame;
    unsigned int  Frame;
} UN_CAN_RFS;

typedef union
{
    unsigned char   uc_Data[72];
    unsigned int        uw32_Data[2];
    ST_CANMsg          m_BMSData;
} UN_CANWORD;

typedef struct
{
    UN_CAN_TFI  TxFrame; 	// Bits 16..19: DLC - Data Length Counter      0..9ID索引
    unsigned int     MsgID;	// CAN Message ID (11-bit or 29-bit)
    UN_CANWORD  Data;	// CAN Message Data

} CAN_TXREG;

typedef struct  						// 这里51和DSP全然不同。  帧数据
{
    unsigned int       TxID;
    unsigned char len;
    unsigned char FrameType;
    unsigned char databuf[8];
    unsigned char TxStatus;				// 初始化时全部置0. 1表示有数据
} ST_CAN_TxMsg;

struct CAN_RXREG						// 这里51和DSP全然不同。  帧数据
{
    unsigned int       RxID;
    UN_CAN_RFS    RxFrame;
    UN_CANWORD    RxData;	                // CAN Message Data Bytes 0-7

};

//  MCU UDP   Begin
typedef union
{
    struct
    {
        unsigned int  INDEX:10;
        unsigned int  BP:1;
        unsigned int  Rsvda:5;
        unsigned int  DLC:4;
        unsigned int  Rsvdb:10;
        unsigned int  RTR:1;
        unsigned int  FF:1;
    } BitFrame;
    unsigned int  Frame;
} UN_CAN_RFS_2;
typedef union
{
    unsigned char   uc_Data[12];
    unsigned int        uw32_Data[3];
} UN_CANWORD_2;
struct CAN_RXREG_2						// 这里51和DSP全然不同。  帧数据
{
    unsigned int       RxID;
    UN_CAN_RFS_2    RxFrame;
    UN_CANWORD_2    RxData;	                // CAN Message Data Bytes 0-7
};
//  MCU UDP End
struct ST_CAN_RxMSG
{
    unsigned int       RxID;
    unsigned char len;
    unsigned char  FrameType;            //帧类型是否扩展
    unsigned char RTR;                   //远程帧
    unsigned char RxRES;				// 初始化时全部置0. 1表示有数据
    UN_CANWORD    RxData;
};

typedef CIRCULAR_QUEUE(MP_CAN_MSG,CAN_TX_BUF_LENGTH)  CQ_CANTXBUF;
typedef CIRCULAR_QUEUE(MP_CAN_MSG,CAN_RX_BUF_LENGTH)  CQ_CANRXBUF;
typedef CIRCULAR_QUEUE(MP_CANBMS_MSG,CAN_RX_BUF_LENGTH)  CQ_CANBMSRXBUF;
typedef CIRCULAR_QUEUE(ST_CAN_RxMSG,1024) CQ_CANBMSRXBUF2;

typedef struct
{
    unsigned char bufIndexEnqueue;
    int          bufCounter[2];
    ST_CAN_RxMSG buf[2][1024];
    unsigned long CanID_Count;
    unsigned long CanID_Sended[1024];
} St_BMS_Filter; //1712211 hpq
class  CANFunc
{
    //Attribute
public:

    //Status
    static unsigned int CAN1Status;
    static unsigned int CAN2Status;
    static unsigned char   LastMsgTxRes;
    static unsigned char TrigCANTxFlg;        //Trigger ISR Flag
    //Rx and Tx Buffer Interface
    static unsigned int     RxBufIndex;
    static unsigned short   RxBuffStatus;
    static unsigned int     DOICnt;
    static unsigned int     FullCnt;
    static unsigned short SourceBoardID;
    static unsigned char CAN1Init;
    static unsigned char CAN2Init;

    static  St_BMS_Filter         BMS_Filter;
    static  CQ_CANBMSRXBUF2       CQ_CanBmsRxBuf2;
    static  CQ_CANTXBUF           CQ_CanTxBuffer[2];
    static  CQ_CANRXBUF           CQ_CanRxBuffer[3];
    static   ST_Multi_Pack                 m_MutiPackets[MAXCHANNELNO][CAN_MULTI_PACK_BUF_LEN];
    static   ST_AuxMulti_Pack              m_AuxMutiPackets[CAN_MULTI_PACK_BUF_LEN];
    static unsigned int       uw32_TBindex;
    static unsigned short       uw16_ArbitrationCounter;

    static unsigned int       FilterBrdCstID;
    //Built_in CANBMS  Start//
    static unsigned char IsCANPort1_AsBuiltInCANBMS;
    static unsigned char IsCANPort2_AsBuiltInCANBMS;
    //Built_in CANBMS  End//
//private:
    //Attribute

    //Tx Rx TempMessage
    static ST_CAN_RxMSG TempCANRxData;
    static CAN_TXREG TempCANTxData;
    static ST_CAN_RxMSG TempNETtoCANRxData;
    static	CAN_RXREG ReadRxReg;
    static unsigned char CAN1RecordBaudRate;
    static unsigned char CAN2RecordBaudRate;
private:
    //TX RX Process
    static unsigned short CAN_MsgNo;          //Confirm CAN Msg No
    static unsigned char CANSendFlg;          //Tx Buffer Ready Flag

    static unsigned char PushToBufRes;
    static unsigned long CANSendFailedCnt;
    static  unsigned  short SendingCnt;
    static MsTime	CAN_SendAbnTimer;        //Sending OverTime Timer
    //Acceptance filter  ram lookup index
    static CANFilterExtGrp     PrioGrpFilter[4];
    static unsigned int       ACCFRamIdx;

    //protected:

    //Operation
public:
    static void Can_Init(unsigned char CANx,unsigned char BaudRate);
    static unsigned char  CAN_ISRSend(unsigned char CANx);
// 	static unsigned char  CAN_Immediate_Send(unsigned char CANx,const MP_CAN_MSG * can_msg);
// 	static void CAN_Reset(unsigned char CANx);
    static unsigned char CAN_RxProcess(unsigned char CANx);
    static unsigned char  CAN_Push_to_Send(unsigned char CANx,const MP_CAN_MSG * can_msg);

    static unsigned char getBID(void);
    static void InitMyCAN(void);
    static void CAN_Init_BaudRate(unsigned char CANx,unsigned char BaudRate);
    static void CAN_Reset_BaudRate(unsigned char CANx,unsigned char BaudRate);
    static unsigned char NETtoCAN_RxProcess(void);
    static bool IsCANopen(void);
#if(USE_AS_AUXMCU ==0)
    static void CAN_ChangeACCF(void);
//Built_in CANBMS  End//
#endif
private:
    static void CAN_BufferInit(void);

    static unsigned char CAN_TxReadyInfo(unsigned char CANx);
    static unsigned char Can_Rx_Msg(unsigned char CANx);
    static unsigned char  Can_Tx_Msg(unsigned char CANx, unsigned char uc_TBindex,const ST_CAN_TxMsg *TxMsgBuf);

    static void CAN_SetACCF_Lookup(void);
    static void InitFilterID(void);
    static void clear_msg_objects(unsigned char CANx);
    static void CAN_StdGrpFilter(unsigned char CANx,uint32 StdIDFrom,uint32 StdIDTo);
    //	static void CAN_ExtExpFilter(unsigned char CANx,uint32 ExpID);
    static void CAN_ExtGrpFilter(unsigned char CANx,uint32 ExtIDFrom,uint32 ExtIDTo);
    static void CAN_SetACCF( unsigned int ACCFMode );

protected:

};

#endif	// __CAN_H

/******************************************************************************
**                            End Of File
******************************************************************************/



