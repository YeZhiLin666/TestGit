#pragma once
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../../COMMON_OS/LPC4078CORE/I2CNew.h"

#define MAXHEADBUF				8
//#define MAXBYTENUM				20
#define MAXSMBMESSAGE    	8

#define SMBDELAYTIMIE_A    1000      //100us
#define SMBDELAYTIMIE_B		  1        //s
#define SMBDELAYTIMIE_C    60         //s
#define SMB_MAX_BYTE_LENGTH_PER_SIGNAL 64   //max byte length per signal
#define SMB_MVOFFLINE_INTERVAL_TIME    50000      // 5s
#define SMB_MAX_MVOFFLINE_COUNTER   3   
#define SMB_MAX_MVCLEAR_COUNTER   3   

enum
{
    TYPE_NA=0,
    TYPE_WORD,
    TYPE_STRING,
    TYPE_DATE,
    TYPE_SIRIALNUMBER,
    TYPE_BITS_CONTROL
};

//Add by zc 12012017
enum SMB_Status
{
    e_SMB_Normal =0,
    e_SMB_Unsafe =1,
};
//==================

enum
{
    SMB_NOTREADY=0,
    SMB_IDLE,
    SMB_WAIT,
    SMB_LOADSIGNAL,
    SMB_ONLINE,
    SMB_WRITE,
    SMB_I2CENGINE,
    SMB_ERROR,
    SMB_END
};

// enum
// {
// 	I2C_IDLE,
// 	I2C_DOWORK,
// 	I2C_FINISH,
// 	I2C_ERROR
// };

enum
{
    SMB_NA=0,
    SMB_Start_Msg,
    SMB_Write_Msg,
    SMB_Stop_Msg,
};

enum
{
    SMB_DATATYPEA =0,
    SMB_DATATYPEB,
    SMB_DATATYPEC,
};

enum
{
    SMBUS_SEND,
    SMBUS_TRAN,
    SMBUS_FINISH,
    SMBUS_ERROR,
};

enum
{
    FAST,
    SLOW,
    ONE_TIME
}; //add by zc 03/04/2019

enum
{
    SYNC_WITH_IV,
    SYNC_WITH_REFRESH
}; //add by zc 03/04/2019

enum
{
    LASTPOINT,
    NOTLASTPOINT
}; //add by zc 04/01/2019
enum
{
    SEND_LOG_SMB_DATA,
    SEND_IDLE_SMB_DATA
};//add by zc 03/04/2019
typedef struct
{
// 	SMB_METACODE    	 m_SMB_RX_Meta;   //enum
// 	unsigned int       m_SMBMessageID;
// 	unsigned char      m_StartDataIndex;   //64BIT ??Index
// 	unsigned char      m_EndDataIndex;    //64BIT ??Index
// 	unsigned char      m_ByteOrder;
// 	unsigned long long m_lowValidBit;      //??????
// 	float              m_MinValue;
// 	float              m_MaxValue;
// 	float              m_Offset;
// 	float              m_ScaleFactor;
// 	bool               m_DataLog;
// 	unsigned char      m_DataType;
    unsigned char    m_SMB_RX_Meta;   //enum
    unsigned char 	m_SignalID;
    ST_COMMHEADER 	m_HeaderBuf;
    unsigned char 	m_Enable;
    unsigned char 	m_Datalog;
    unsigned char 	m_DataType;
// 	unsigned char 	m_Code;
// 	unsigned char 	m_Access;
// 	unsigned char 	m_Initial;
    unsigned char 	m_unit;
    unsigned char 	m_PECEnable;
    unsigned char   m_DataLen;
    unsigned char   m_Endian;
    unsigned int  	m_StartDataIndex;
    unsigned int  	m_EndDataIndex;
    unsigned char  	m_StartByteIndex;
    unsigned char  	m_EndByteIndex;
    unsigned char  	m_StartBitIndex;
    unsigned char  	m_EndBitIndex;
    unsigned char   m_Sign;
    float						m_Gain;
    float						m_Offset;
    float  					m_Refresh;
    unsigned int  	m_Logging_Interval;              //SMBus打点速度 	add by zc 03.04.2019
} St_SMBBMS_ParsedMeta;

typedef struct
{
    St_SMBBMS_ParsedMeta m_ValidSignal[MAX_SMB_BMS_SIGNAL];
    unsigned char 			 m_WholeSignalNum;
    unsigned char        m_MetaCount;
    bool 		            m_bEnable;       // this SMB bms channel is disable?
    unsigned short       m_RegisterBit;
    St_MD5               m_MD5;
    MsTime               m_AbondonTime;
    unsigned char        m_Endian;
// 	unsigned char        m_bBrdCstUpdate;
// 	unsigned char        m_bBrdCstValid;
	unsigned int        m_BaudRate;   //SMBBAUD250k = 2,SMBBAUD500k = 3,SMBBAUD1M = 4
// 	unsigned char        m_FrameType;  //broadcast frametype
// 	unsigned int         m_BrdCstID;
// 	unsigned int         m_BrdCst_interval;
    unsigned int         m_UnsafeTimeout;
    unsigned int  	m_Refresh_Fast;    //FAST用户设定记录速度 	add by zc 03.04.2019
    unsigned int  	m_Refresh_Slow;    //SLOW用户设定记录速度 	add by zc 03.04.2019
} St_SMBBMS_Config;

typedef struct
{
    unsigned char      m_bufCnt;
    St_SMBBMS_Config   m_buf[MAX_LEN_SMB_CONFIG];
} St_SMBBMS_ConfigBuf;
typedef struct
{
    unsigned char       m_IVChnNo;
    int                 m_cfgBufIdx;
    int                 m_insertPos;
    St_MD5              m_MD5;
} St_SMBConfigAssignTab;

//add by zc 12012017
typedef struct
{
    unsigned char  m_bCANBusAlive;
    unsigned char  m_SMBusStatus;
    MsTime         m_UnsafeTime;
    MsTime         m_SubMsgOvertime;
    MsTime         m_SendIdleDataTick;
    MsTime         m_IdleDataRptTick;
    MsTime         m_SMBus_RptLimitTick;
} St_SMBUnsafeInfo;

typedef struct
{
    unsigned char  m_type;
    short  m_Word;
    unsigned char  m_GetBuf[30];
    unsigned char  m_Len;
    unsigned char  m_Endian;
    unsigned int	 m_EndDataIndex;
    unsigned int	 m_StartDataIndex;
    unsigned char  m_EndByteIndex;
    unsigned char  m_StartByteIndex;
    unsigned char  m_EndBitIndex;
    unsigned char  m_StartBitIndex;
} St_SMBData;
typedef struct
{
    unsigned char MsgNumber;
    unsigned char m_TimeInterval; //间隔时间
    unsigned char m_Respond;   //响应种类  0/单次  1/循环
    unsigned char m_bufLen;
    unsigned char m_HeaderAddr;
    unsigned char m_SlaveAddr;
    unsigned char Send_buf[MAXBYTENUM]; //信息包含发送字节
} St_SMBMessage;


typedef struct
{
    unsigned char  Used;
    St_SMBMessage SMBMessage;
} St_DoSMBMessage;         //需要处理的Message结构


typedef struct
{
    st_I2CModule 	m_Header;
    unsigned short SubBufEndingAdr;
    unsigned char m_Comm;
} ST_HeaderPackage;
typedef struct
{
    ST_HeaderPackage	m_Headbuf[MAXHEADBUF];
    unsigned char m_len;

} ST_SMBSENDHeader;


typedef struct
{
    unsigned long int  m_IVChnIdx;              //对应的通道编号
    unsigned char      m_ItemCount[MAXCHANNELNO];
    ST_SMBBMS_CfgUnit  m_BMS_Chn[MAXCHANNELNO];
} ST_SMBBMS_CONFIG_CH;


class Smart_Battery
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级

    static unsigned long  						I2Ctimeout;
    static bool												BeSupportSmbus;
    static bool           						SMB_RunFlg;
    static bool												ErrorFlg;
    static unsigned long  						OfflineCounter;
    static bool												OfflineFlg;

    static St_SMBBMS_ConfigBuf     		m_SMBCfgBuf;
    static St_SMBConfigAssignTab  		m_SMBAssignMD5[MAXCHANNELNO];
    static unsigned long      m_SMBSignalErrorCounter[MAXCHANNELNO][MAX_SMB_BMS_SIGNAL];
    static St_SMBUnsafeInfo   		m_SMB_OverTime[MAXCHANNELNO];  //add by zc 12012017
    static unsigned char					SMBus_InitDelayCounter;
    static ST_SMBBMS_CONFIG_CH						DownLoadConfig;
    static unsigned char 							m_OptWordAddress;
    static unsigned char              m_SMBUnitID;
//		static bool   										be_Interrupt;  //comment out by zc 06/05/2018
    static MsTime 										m_RefreshCounter[MAXCHANNELNO][MAX_SMB_BMS_SIGNAL];
    static bool            						m_UpdataFlg[MAX_SMB_BMS_SIGNAL];
    static MsTime											m_PreviousLogUploadTime[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
    static MsTime											m_PreviousIdleUploadTime[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
    static unsigned char      				m_SMBLogIdx;
    static bool FirstLog_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
    static bool LastLog_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
    static bool FirstRefresh_Flag[MAX_SMB_BMS_SIGNAL];//add by zc 03.04.2019
    static MsTime LastIVtime;
	static MsTime     MVOffline_IntervalTime;
	static unsigned char uc_MVOffline_statuse;
    static unsigned char uc_MVOffline[MAX_SMB_BMS_SIGNAL];
	static bool lastSMBData[MAXCHANNELNO];
public:
    static void        		DoWork(void);                                                                 //任务执行函数
    static void        		MyInit(void);                                                                 //任务初始化
// 		static void        		EndTask(void);                                                                //任务结束
private:
    static St_SMBData					m_SMBData;

    static unsigned char 			WriteSMBBuf[MAXBYTENUM];
    static St_MsgBufManage    MsgBufManage;
    static St_DoSMBMessage   	DoSMBMessage;    						//建立当前处理的信息缓冲
    static St_SMBMessage			SMBMsgBuf[MAXSMBMESSAGE];

    static unsigned char 			m_SMBusStatus;
    static unsigned char      m_SMBEngineStatus;          //SMB引擎状态
    static unsigned char 			m_ReadDataType;             //读数据类型
    static unsigned long      m_RefreshInterval[MAX_SMB_BMS_SIGNAL];


// 	  static MsTime   									m_RecordReadDelayTimeA;
// 	  static MsTime   									m_RecordReadDelayTimeB;
// 		static MsTime   									m_RecordReadDelayTimeC;
    static unsigned char 			m_ReturnNextStatus;



    static St_SMBBMS_ParsedMeta 	m_CurrentSMBSignal;
    static unsigned char 			m_CurrentBufIdx;


public:
    static int 					  ParseSMBMessage(void);
    static void						ProcessSMBStatus(void);
    static St_SMBBMS_ParsedMeta FindNextSignal(void);
    static int 						SendMsgToSMB(St_SMBMessage SMBMessage);
    static void 					GetSMBMsg(unsigned char MsgIndex);
    static void						ClearCurrentSMBMsg(St_DoSMBMessage *m_msg);
    static void 					SMBIdleRoutine(void);
    static void 					SMBWaitRoutine(void);
    static void 					SMBLoadSignalRoutine(void);
    static void						SMBOnlineRoutine(void);
    static void						SMBWriteRoutine(void);
    static void 					SMBErrorRoutine(void);
    static unsigned char 	I2CEngine(unsigned char I2Cx,unsigned char m_ReturnStatus,St_SMBData m_SMBData);
    static unsigned char	SMBEngineRoutine(unsigned char uc_ChannelIdx,unsigned char I2Cx,unsigned char m_SMBGotoStatus,St_SMBData m_SMBData);
    static Word16 				Get_SmartBatteryWord(St_SMBBMS_ParsedMeta  m_Signal);
// 		static void						Write_SmartBatteryByte(unsigned char Comm,unsigned char m_Len,unsigned char *m_byte);
    static void 					Get_SmartBatteryString(unsigned char m_StrLen,St_SMBBMS_ParsedMeta  m_Signal,unsigned char *m_Str);
    static bool 					Assign_SMBConfig(unsigned char uc_ChannelNo);
    static bool 					Clear_SMBConfig(unsigned char uc_ChannelNo);
    static unsigned char  I2CEngine_Exclusive(unsigned char I2Cx);
    static void 					SetSMBusFirstLog(); //add by zc 04/01/2019
    static void						SetSMBusLastLog(unsigned char uc_ChannelNo, unsigned char IfLastPoint); //add by zc 04/01/2019
    static void 					ResetSMBusFirstLog(unsigned char SMBMessage_Send_buf_0); //add by zc 04/01/2019
    static void 					UploadSMBusData(unsigned char uc_i, ST_SMBDATABUF mSmbBufDataLog, unsigned char DataType);//add by zc 04/01/2019
		static void                     MVOffline_SetFlag(void);
		static void                     Clear_AllSBMetaVariable(void);
};

