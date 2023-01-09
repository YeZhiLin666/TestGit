#ifndef __I2C_NEW_H
#define __I2C_NEW_H

#define MAX_I2C_NUM     3
#define MaxRequestNum   40
#define MAX_FRAMELEN    64
#define MAXI2CBUFLEN    1024
#define MAXI2CHANDLE    500
#define MAXI2CRETRY     2



#define MAX_I2C_BUFFER  64
#define MAX_I2C_COUNT    2

//i2c program

#define I2CMASTER		       0x01      //主机
#define I2CSLAVE		       0x02      //从机

/* I2C  State*/
#define I2C_IDLE			           0         // 闲置的
#define I2C_STARTED			           1         // 开始的
#define I2C_RESTARTED		           2         // 重新启动
#define I2C_REPEATED_START	           3         // 重复的
#define DATA_ACK			           4         // 数据应答
#define DATA_NACK			           5         // 没有数据应答
#define READ_PCA9500MEMORY             6         // 读内存数据

/* I2C Control Set Register */
#define I2CONSET_AA			         0x00000004      // 应答标志
#define I2CONSET_SI			         0x00000008      // 中断标志
#define I2CONSET_STO		         0x00000010      // 停止标志
#define I2CONSET_STA		         0x00000020      // 开始标志
#define I2CONSET_I2EN		         0x00000040      // 使能标志

/* I2C Control clear Register */
#define I2CONCLR_AAC		         0x00000004       // 应答标志
#define I2CONCLR_SIC		         0x00000008       // 中断标志
#define I2CONCLR_STAC		         0x00000020       // 开始标志
#define I2CONCLR_I2ENC		         0x00000040       // 使能标志

/* I2C  Register Initialize*/
#define I2ADR_I2C			         0x00000000  /* I2C Slave Address Reg */
#define I2DAT_I2C			         0x00000000  /* I2C Data Reg */
#define I2SCLH_SCLH			         0x00000080  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL			         0x00000080  /* I2C SCL Duty Cycle Low Reg */

#define I2CBUFSIZE			   0X20              //使用I2C时，最大的缓冲区
#define I2CMAX_TIMEOUT	       0x000FFFFF       //使用I2C时，最大的超时时间

#define I2CBUFSIZE			   0X20              //使用I2C时，最大的缓冲区
#define I2CMAX_TIMEOUT	       0x000FFFFF       //使用I2C时，最大的超时时间




#define _New_I2cBus  1
typedef struct
{
    unsigned char I2Cx;
    unsigned short I2CSpeed;
    St_Port I2C_SCKPort;
    St_Port I2C_SDLPort;
    unsigned long TransDelaytime;
} st_I2CInitConfig;

#pragma pack(1)
typedef union
{
    struct
    {
        unsigned char SlaveAdr;
        unsigned long long WRRegAdr;
    } SLA_Adr;
    unsigned char CtrlByte[9];
} st_CtrlByte;
#pragma pack()
typedef struct
{
    unsigned char I2Cx;
    unsigned char OPMode;   //(I2C_OPTYPE)
    unsigned char CtrlByteNum;    //?????????
    unsigned short HandleID;
    unsigned short PageSize;
    st_CtrlByte MasterCtrlByte;//slave??,Fram??
    unsigned int  BufIdx;
    unsigned int  BufLen;
    unsigned char Buf[MAXI2CBUFLEN];
} st_I2CModule;


typedef struct
{
    unsigned char I2C_TaskState;
    unsigned char RequestCnt;   //请求数
    unsigned char SubPackNum;    // 子包数
    unsigned char SubPackLen;    //子包长度
    unsigned char CurrentPackNo; //当前子包号
    unsigned short SubPackStartIdx;
    unsigned short SubBufEndingAdr;
    unsigned char CurrentAdr[4];   //address
    unsigned short CurrentHandle;//当前的handleID
    unsigned short PageSize;    //当前器件的页大小
    unsigned char RetryCnt;     //子块重试请求次数

} st_I2CTaskManager;

typedef struct
{
    MsTime StartOvertime;
    MsTime StopOvertime;
    MsTime TransOvertime;
    MsTime NextTransDelay;
    MsTime HoldingTime;
} st_I2COverTime;

typedef enum
{
    I2CBus_IDLE = 0,
    I2CBus_START,
    I2CBus_STARTED,
    I2CBus_STARTFAILED,
    I2CBus_ACK,
    I2CBus_NACK,
    I2CBus_STOPPED,
} I2C_BusState;

typedef struct
{
    unsigned char SlaveAdr;       //从机地址
    unsigned char CtrlByteNum;   //控制字节数
    unsigned char CtrlByteIdx;
    unsigned char StopCondition;
    unsigned char I2CBusy;   //立即请求锁
    unsigned short HandleIdx;
    st_I2COverTime Overtime;
    I2C_BusState  BusState;

} st_I2CBusManager;

enum I2C_MODULE
{
    I2C0 = 0,
    I2C1,
    I2C2
};
enum I2C_BUSYTYPE
{
    I2C_NOT_BUSY,
    I2C_IMME_BUSY,  //直接处理busy
    I2C_TASK_BUSY,

};
enum I2C_TASK_STATE
{
    I2C_Task_Idle = 0,
    I2C_Task_DataTransfer,
    I2C_Task_PrepareNext,
    I2C_TASK_DataHold,
    I2C_Task_RetryTrans,
    I2C_Task_ReportError,
};


enum I2C_SPEED
{
    I2C_100K = 100000,
    I2C_400K = 400000,
};


enum I2C_OPTYPE
{
    I2C_NOWORK,
    I2C_MASTWRITE,
    I2C_MASTREAD,
    I2C_SLAVEWRITE,
    I2C_SLAVEREAD,
};
enum I2C_WRBIT
{
    I2C_WriteBit = 0,
    I2C_ReadBit = 1,
};
enum I2CStopCondition
{
    I2CNormalStop = 0,
    I2CAbnormalStop
};
enum I2C_IRQ_STATE
{
    IRQ_BUS_ERR = 0x00,
    IRQ_STARTED = 0x08,
    IRQ_RESTARTED = 0x10,
    IRQ_MSTTX_SLA_ACK = 0x18,
    IRQ_MSTTX_SLA_NACK = 0x20,
    IRQ_MSTTX_DATA_ACK =  0x28,
    IRQ_MSTTX_DATA_NACK =  0x30,
    IRQ_MSTRX_SLA_ACK =  0x40,
    IRQ_MSTRX_SLA_NACK =  0x48,
    IRQ_MSTRX_DATA_ACK =  0x50,
    IRQ_MSTRX_DATA_NACK =  0x58,

};
//需要替换I2C.h
/*
enum I2C_CONSET_REG
{
// I2C Control Set Register
I2CONSET_AA   = 0x00000004,   // 应答标志
I2CONSET_SI   = 0x00000008,     // 中断标志
I2CONSET_STO  = 0x00000010,   // 停止标志
I2CONSET_STA  = 0x00000020,   // 开始标志
I2CONSET_I2EN = 0x00000040,   // 使能标志
}
enum I2C_CONCLR_REG
{
// I2C Control clear Register
I2CONCLR_AAC   = 0x00000004,      // 应答标志
I2CONCLR_SIC   = 0x00000008,   // 中断标志
I2CONCLR_STAC	 = 0x00000020,    // 开始标志
I2CONCLR_I2ENC = 0x00000040,     // 使能标志
}
*/

enum I2C_ERROR
{
    I2C_START_ERR = 0,
    I2C_STOP_ERR,
    I2C_RX_OVERTIME_ERR,
    I2C_TX_OVERTIME_ERR,
    I2C_BUS_ERROR,
    I2C_BUSY,
    I2C_NACK_ERR,//Master ??
};

//用于Handle的状态回复  //request
typedef enum
{
    I2C_ReqNone = 0,
    I2C_ReqRdy,
    I2C_ReqRun,
    I2C_ReqFin,
    I2C_ReqErr,
    I2C_ReqHandled,//已处理
    I2C_ReqDestroy,//已处理完，丢弃
} I2C_Handle_Status;
typedef struct
{
    unsigned short m_ID;
    I2C_Handle_Status m_Status;
} I2CRequestHandle;

typedef CIRCULAR_QUEUE(st_I2CModule,MaxRequestNum)  CQ_I2CBUF;
typedef CIRCULAR_QUEUE(I2CRequestHandle,MAXI2CHANDLE)    I2CHandleQueue;

class I2CBusNew
{
public :
#if  USE_AS_AUXMCU == 1
    static UInt16 Priority;
    static UInt16	TaskID;
#endif
    static st_I2CTaskManager m_I2CTaskMgr[MAX_I2C_NUM];
    static st_I2CBusManager m_I2CBusMgr[MAX_I2C_NUM];
    static st_I2CInitConfig I2CConfig[MAX_I2C_NUM];
    static st_I2CModule RunningBuffer[MAX_I2C_NUM];
    static CQ_I2CBUF m_I2CBuf[MAX_I2C_NUM];
    static I2CHandleQueue    m_qI2CHandle;
    static unsigned short HandleIdx;
    static const St_Port I2C1_SDA_IOCFG;
    static const St_Port I2C1_SCL_IOCFG;

public:
#if  USE_AS_AUXMCU == 1
    static void TaskInit();
    static void DoWork();
    static unsigned short I2C_Task_Request(unsigned char OpMode, st_I2CModule * pSource);
#endif
    static void DoI2CTask(unsigned char I2Cx);
    static I2C_Handle_Status GetRequestStatus(unsigned short Handle);
    static void SetRequestStatus(unsigned short Handle,I2C_Handle_Status status);

    static bool  I2C_ImmidiateTrans( st_I2CModule * pSource);
    static unsigned char  I2CInit (unsigned char I2Cx,unsigned short BaudRate);
    static void ProcessISRstate(unsigned char I2Cx);
    static bool AD5245_SET(unsigned short adr,unsigned char data, unsigned char I2C_Port);  //add by qjm20141011 控制AD5245电位器

    //test i2c request
    //static unsigned short I2C_ReadTest(unsigned short adr,unsigned short Len);
    //static unsigned short I2C_WriteTest(unsigned short adr,unsigned char iData,unsigned short Len);
    //static void LM75BTEST(unsigned char I2Cx,unsigned char  adr);
    static bool I2CStart(unsigned char I2Cx);//Moved from private to public by ZC 09/29/2016
    static bool I2CStop( unsigned char I2Cx);//Moved from private to public by ZC 09/29/2016

private:
//	static bool I2CStart(unsigned char I2Cx);
//	static bool I2CStop( unsigned char I2Cx);
    static void  InitFirstBuf(st_I2CModule *I2CMoudle);
    static I2C_BusState DetectBusState(unsigned char I2Cx);
    static bool PrepareNextBuf(st_I2CModule *I2CMoudle);
    static void RetryCurrentBuf(unsigned char I2Cx);

private:
    static const int const_StartOvertime;
    static const int const_StopOvertime;
};

#endif

