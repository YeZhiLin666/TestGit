//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>CANParse Configuration
// =====================
//
#ifndef  __CANPARSE_H
#define  __CANPARSE_H

//#define RX_EMPTY                    0
//   <q>Aux Status Report Enable:
//   <i> Default: On
#define  USE_AUX_STATUS_REPORT       1

// <o>IV Check Aux Status Tick <1-10000>
//   <i> Default: 200ms  [100us Unit]
#define  IV_CHECK_AUX_TICK        11000 //5000

// <o>Aux Msg Lost Max Count <1-100>
//   <i> Default: 1S
//   <i> Value 10 time(IV_CHECK_AUX_TICK *10)
#define  MAX_CAN_LOST_COUNT     10 //10*200ms

// <o>Aux Status Info Max Error Count <1-10>
//   <i> Default: 4
#define  MAX_AUX_STATUS_ERR_COUNT   MAX_CAN_LOST_COUNT //4

// <o> Aux Report Status Info Tick <100-5000>
//   <i> Default: 500ms  [100us Unit]
#define  AUX_REPROT_INFO_TICK         10000 // 5000 //500ms

// <o> Parallel Update Data Ticks
//  <i> Default: 20ms  [100us Unit]
#define   PARA_UPDATEOUT_TICK          200
#define   PARA_UPDATEOUT_TICK_RAPIDLY_COMMUNICATE         0  // 保持最快发送，不考虑时间

//#define  USE_CAN_TIME_SYNC           1
#define  TIME_PRECISION             10   //1ms


//CAN BMS CONFIG_
//be careful,same sequence with the following line:
//   <o>CAN BMS PORT   <0=> CAN1     <1=> CAN2
//   <i> Default:    CAN2  (according to CAN.h)  CAN_PORT_USE_EXCLUSIVE
#define CAN_BMS_PORT              1

// <o> CAN_BMS Config Buf Size
// <i> Default: 20
#define MAX_LEN_CAN_CONFIG        20

#define CAN_ORGCODE               0xFFFFFFFFFFFFFFFF  //CAN BMS PARSE CODE
// <o> CAN_BMS Report Data to PC Min Interval
// <i> Default: 500ms
#define CAN_BMS_DATA_RPT_FAST_INTERVAL   5000

// <o> CAN_BMS Report Data to PC Mid Interval
// <i> Default: 1s
#define CAN_BMS_DATA_RPT_MID_INTERVAL    10000

// <o> CAN_BMS Report Data to PC Slow Interval
// <i> Default: 30s
#define CAN_BMS_DATA_RPT_SLOW_INTERVAL   30

// <o> CAN_BMS Report Idle Data to PC Min Interval
// <i> Default: 3s
#define CAN_BMS_NOTLOGGED_RPT_INTERVAL    30000

#define BMS_BroadCst_Min_Period          50

#define BMS_CANMsg_Min_Period            20
#define MAX_CAN_BMS_ACK_BUF        		 100
#define MAX_MULTI_PACK_OVERTIME         70
#define MAX_MULTI_PACK_ACK_OVERTIME        (10*(800/OS_TICK_INTERVAL))    //7ms 


#ifndef MXUNITNO
#define MAXUNITNO			40		//最多40 unit/system
#endif

#define SUBMASTER_SN_RES_COUNT    10

//CAN网类型
//#define INTERACANNET 0x01
//#define OUTERCANNET  0x02
//#define IV4AUX       0x04

typedef struct
{
    unsigned char ID;
    unsigned char ID_Ready;
} IV_MCU_ID;

typedef struct
{
    unsigned short m_MsgSN;
    unsigned short m_LimitSN;
    unsigned short m_AnyLimitSN;
    bool           m_bResendPara;
    unsigned short ParaSN;
    unsigned short m_CANBMS_SN;
} ST_CAN_SN;

typedef struct
{
    float m_Vpp;
    float m_Vmm;
} ST_M0_Value;
typedef struct
{
// 	unsigned char m_Type;
    float Vpp_Gain;
    float Vpp_Offset;
    float Vmm_Gain;
    float Vmm_Offset;

} Power_DACGainOffset;
//#if(SUPPORT_AUX==1)
enum
{
    SAFETYLOW=0,
    SAFETYHIGH,
    SAFETYVoltageLow,
    SAFETYVoltageHigh,
    SAFETYCurrentLow,
    SAFETYCurrentHigh,
    SAFETYTemperatureLow,
    SAFETYTemperatureHigh,
};
enum
{
    BMS_FIGURE=0,
    BMS_METAVARIABLE,
    BMS_FORMULA,
};
enum
{
    SINGLECHANNEL=0,
    ALLMAPCHANNEL,
};

enum
{
    OS_TIME_PLUS,
    OS_TIME_MINUS,
};
//1 error ,0 normal
enum
{
    AUX_STATUS_NORMAL = 0x00,   //any bit set 1 is abnormal
    AUX_COUNT_ERROR = 0x02,     //judge error :&AUX_COUNT_ERROR    Set error :|AUX_COUNT_ERROR
    AUX_COUNT_NORMAL = ~AUX_COUNT_ERROR,   //Set Normal: &=AUX_COUNT_NORMAL
    AUX_CAN_DISCONNECTED =4,
    AUX_CAN_CONNECT_NORMAL =~AUX_CAN_DISCONNECTED,
};
//Built_in CANBMS  Start//
enum
{
    USE_AS_NORMAL_CAN, //通用方式使用，CAN1公有，CAN2私有。
    USE_AS_BUILT_IN_CANBMS,	//只能用做Built_inCANBMS.
};
//Built_in CANBMS  End//
typedef struct
{
    unsigned char Count;
    unsigned char Connect;
    unsigned char Ack;
    unsigned char Status;
} ST_AuxUnitStatus;

typedef struct {
    unsigned char  m_bExist;
    unsigned char  m_auxID;
    //unsigned char  m_auxType[MAXAUXTYPE];
    unsigned char  m_lostCnt;
    unsigned char  m_errorCount;
    unsigned char  m_auxAlive;
    unsigned char  m_startAskFlag;
    unsigned char  m_askedCount;
    unsigned char  m_bShiedCheck;
    unsigned char  m_auxStatus;
    unsigned char m_bRefreshStatus;
    MsTime        m_IVAskTick;
    MsTime        m_checkAuxInfoTick;
} ST_IVAskTable;


typedef struct
{
    int  Second;
    int  Us100;
    unsigned char polarity;
} St_TimeDiff;

typedef struct
{
    unsigned char   m_bIVAskFlag;
    unsigned char   m_IVChnExist;
    unsigned char   m_CurrentAuxSum[MAXUNITID];
} St_AuxInfoRes;

typedef struct
{
    unsigned char   m_bRptInfo[MAXCHANNELNO];
    unsigned char   m_UnRptCnt[MAXCHANNELNO];
    unsigned char   m_bIVAskFlg;
    MsTime          m_RptTick;
} St_AuxStatusRpt;

typedef enum
{
    CLRLIMIT_NONE,
    CLRLIMIT_WAIT,
    CLRLIMIT_OK,
} ClrLimitState;

typedef struct
{
    unsigned short m_SN;
    unsigned char  m_AuxID;
    unsigned char  m_bValid;
    ClrLimitState  m_Check;
} St_ClrLimitInfo;
typedef struct
{
    St_ClrLimitInfo m_AuxInfo[MAXAUXUNIT];
    unsigned short StepID;
} St_ClrLimitAck;

//#endif

////////////////SUPPORT_CAN_BMS////////////////////
//#if(SUPPORT_CAN_BMS == 1)

typedef struct
{
    CANBMS_METACODE    m_CAN_RX_Meta;   //enum
    unsigned int       m_CANMessageID;
    unsigned int       m_StartDataIndex;   //64BIT ??Index
    unsigned int       m_EndDataIndex;    //64BIT ??Index
    unsigned char      m_ByteOrder;
    unsigned char      m_DLC;
    unsigned long long m_lowValidBit;      //??????
    float              m_MinValue;
    float              m_MaxValue;
    float              m_Offset;
    float              m_ScaleFactor;
    unsigned char      m_DataLog; //20170606 zyx
    unsigned char      m_DataType;
} St_CANBMS_ParsedMeta;

typedef struct
{
// 	unsigned int         m_BrdCst_ID; //广播ID
// 	unsigned char        m_FrameType;//标准帧还是扩展帧
// 	unsigned char        m_Endian;	//大小头选择
// 	unsigned int         m_BrdCst_interval; //广播时间间隔(ms)
// 	unsigned char        m_Data_Mode[2];//数据模式 0:figure,1:MetaVariable,2:Formula?
// 	float                m_BrdCstData[2];//figure，MetaVariable代码或公式序号

    ST_CAN_BroadcastInfo   m_Info;

    unsigned char        m_bBrdCstUpdate;//广播更新标志
    unsigned char        m_bBrdCstValid;//广播有效标志
} St_CANBMS_BrdCstInfo; //一组广播数据
typedef struct
{
	unsigned long        m_CANID;
	unsigned long        m_MemberCount;
	St_BMS_CFG_OutboundMessage   m_MsgMember[8];
}St_CANBMS_OutboundMsgInfo;
typedef struct
{
    St_CANBMS_ParsedMeta m_ValidSignal[MAX_CAN_BMS_SIGNAL];
    ST_CANBMS_ITEMS      m_MetaCount; //char 1705051 zyx
    bool 		         m_bEnable;       // this CAN bms channel is disable?
    unsigned short       m_RegisterBit;
    St_MD5               m_MD5;
    MsTime               m_AbondonTime;
    //unsigned char        m_Endian;
    unsigned char        m_AuxCANID;   //source CANID,transfer aux unit
    //unsigned char        m_bBrdCstUpdate;
    //unsigned char        m_bBrdCstValid;
    bool                 m_IsFDCAN;
    unsigned char        m_BaudRate;   //CANBAUD250k = 2,CANBAUD500k = 3,CANBAUD1M = 4
    unsigned char        m_BitRate;
    //unsigned char        m_FrameType;  //broadcast frametype
    //unsigned int         m_BrdCstID;
    //unsigned int         m_BrdCst_interval;
    unsigned int         m_UnsafeTimeout;
    St_CANBMS_BrdCstInfo         m_BroadCastInfo[BMS_BROAD_CST_COUNT];//3组广播信息
    St_FormulaData       m_FormulaData[16];//CAN config中16组公式
	  bool                 m_FormulaReflash[16];//公式值状态
	  float                m_FormulaValue[16];
	unsigned int               m_CAN_OutboundSignalCount;   //190617,hpq
	unsigned int               m_CAN_OutboundMsgCount;      //190617,hpq
	ST_BMS_CFG_OutboundSignal  m_CAN_OutboundSignals[MAX_CAN_BMS_OUTBOUNDSIGNAL];    //190617,hpq
	St_CANBMS_OutboundMsgInfo  m_CAN_OutboundMsgInfos[MAX_CAN_BMS_OUTBOUNDMESSAGE];  //190617,hpq
} St_CANBMS_Config;

typedef struct
{
    unsigned char      m_bufCnt;
    St_CANBMS_Config   m_buf[MAX_LEN_CAN_CONFIG];
} St_CANBMS_ConfigBuf;
typedef struct
{
    unsigned char       m_IVChnNo;
    int                 m_cfgBufIdx;
    int                 m_insertPos;
    St_MD5              m_MD5;
} St_CANConfigAssignTab;

typedef struct
{
    unsigned char        m_Endian;
    unsigned char        m_FrameType;
    unsigned  char       m_bValidBrdcst;
    unsigned int         m_BrdCst_ID;
    MP_CAN_MSG           m_BradCastMsg;
} St_CANBMS_AuxBrdCst;
typedef struct
{
    unsigned char        m_IVCANID;       //aux get this iv unit id
    bool                 m_bAuxForward;   //enable aux unit forward bms msg;
    bool 								 m_IsFDCAN;
    unsigned char        m_BaudRate;        //CANBAUD250k = 2,CANBAUD500k = 3,CANBAUD1M = 4
    unsigned char        m_BitRate; //181225 zyx
    St_CANBMS_AuxBrdCst  m_BrdCstInfo[BMS_BROAD_CST_COUNT];    //broadcast send info

    unsigned  char       m_AllStop;      //can msg send global info
    unsigned char        m_validMsgCnt;
    struct
    {
        MP_CAN_MSG     m_CANMsg;
        unsigned long long m_Period;	      //20150227,hepeiqing,unsigned short will set a time limit as 65535/10 ms。
        unsigned  char m_SignalStop;
        unsigned char  m_bUpdate;
        unsigned char  m_bValid;
        unsigned char  m_bBroadcast;
        MsTime         m_Overtime;
    } m_Msg[SEND_CAN_BMS_MAX_LEN+1];
} St_CANBMS_AuxInfo;

enum CAN_BMS_Status
{
    e_BMS_Normal =0,
    e_BMS_Unsafe =1,
};

typedef struct
{
    MsTime m_CANBMS_FastRptLimitTick;
    MsTime m_CANBMS_MidRptLimitTick;
    MsTime m_CANBMS_SlowRptLimitTick;
} CANBMS_RPT_TICK;

typedef struct
{
    unsigned char       m_bCANBMSAlive;
    unsigned char       m_BMSStatus;
    MsTime              m_UnsafeTime;
    MsTime              m_SubMsgOvertime;
    MsTime              m_SendIdleDataTick;
    MsTime              m_IdleDataRptTick;
    CANBMS_RPT_TICK     m_CANBMS_RptLimitTick;
} St_BMSUnsafeInfo;

typedef struct
{
    MP_CAN_MSG     m_CANMsg;
    unsigned int   m_Period;
    unsigned char  m_SignalStop;
    unsigned char  m_StopSigTrig;
    unsigned char  m_bBroadcast;
    unsigned char  m_bUpdated;
    unsigned char  m_bValid;
    unsigned int   m_SendTick;
    MsTime         m_CurrentTick;
    unsigned char        m_Data_Mode[2];//数据模式0：数字，1:Metavariable,3Formula.
    float          m_BrdCstData[2];//数字、Metavariable或公式代码
	signed short   m_OutboundMsgInfoIdx; //190617,hpq
} St_BMS_SendUnit;
typedef struct
{
    unsigned char   m_validMsgCnt;
    unsigned char   m_glbStopAll;
    //unsigned char   m_bNewTest;
    St_BMS_SendUnit m_Msg[SEND_CAN_BMS_MAX_LEN+1];  //20150226,hepeiqing,SEND_CAN_BMS_MAX_LEN == 19;

} St_CANBMS_SendBuf;

typedef struct
{
    unsigned int   m_Period;
    unsigned char  m_AllStop;
    unsigned char  m_SignalStop;
    unsigned char  m_DestID;
    unsigned  char m_SendAll;
    unsigned char  m_StopAll;
    unsigned char  m_bStopAllTrig;
    unsigned char  m_StopSignal;
    unsigned char  m_StopSigTrig;
    unsigned char  m_bBroadcast;
    unsigned char  m_Endian;
    unsigned char  m_bOnceTime;
} St_BMS_SendInfo;

enum en_BMS_MsgAckStatus
{
    en_AckNULL = 0, //initial status
    en_AckWait = 1,
    en_AckOK = 2,
};
enum en_BMS_ChnStatus
{
    en_BMS_ACK_Safe = 0,
    en_BMS_ACK_Unsafe = 1,
};
typedef struct
{
    unsigned char m_AckStatus;
    unsigned char m_SN;   //multiple msg sn(first &second msg)
    unsigned char m_ReTryCnt;
    unsigned int  m_OverTick; //overtime tick (base tick system tick)
    MP_CAN_MSG    m_FirstMsg;
    MP_CAN_MSG    m_SecondMsg;
} St_BMS_BackupUnit;

typedef struct
{
    St_BMS_BackupUnit m_Buf[MAX_CAN_BMS_ACK_BUF];
    unsigned char     m_ChnStatus;
} St_BMS_AckConfirmMgr;
//#endif



#define  PARA_MAX_BUF       5
#define  PARA_MAX_OVERTIME  200*20  // 200*UDP_PARALLEL_WATCHDOGRATE
typedef enum
{
    eParaAckNone = 0,
    eParaAckWait = 1,
    eParaAckOK = 2,
} E_Para_AckStatus;

enum en_Para_SubUnsafeCode
{
    eParaSubOk = 0,
    eParaSubCurUnsafe = 1,
    eParaSubVolUnsafe = 2,
    //eParaSubIrrUnsafe = 3,
	eParaSubIrregularUnsafe=4,	
	eParaSubFan_FaultUnsafe=5,
	eParaSubPowerOffUnsafe=6,
	eParaSubUnknowUnsafe = 7,
};

typedef struct
{
    unsigned char m_MasterID;
    unsigned char m_SubUnitCnt;
    unsigned char m_SubUnitIdx;
    E_Para_AckStatus CmdStatus;
    unsigned short SN;
    unsigned char m_ReSendCnt;
    MsTime     m_OverTime;
    unsigned char m_SubID[MAXUNITNO];
    E_Para_AckStatus m_SubStatus[MAXUNITNO];
    ////目前只有3个需要ack

    MP_CAN_MSG m_Msg;
} St_ParaAckMgr;


typedef struct
{
    struct
    {
        unsigned char Cmd;
        unsigned char bValid;
        unsigned short SN;
    } m_RxRes[SUBMASTER_SN_RES_COUNT];

    unsigned char m_CurrentIdx;

} St_SubMaster_Confirm;

typedef struct
{
    CANBMS_METACODE MetaItemsBufFrom;
    CANBMS_METACODE MetaItemsBufTo;
} MetaItemsBufAddr; //170606 zyx

typedef struct
{
    unsigned char        m_bValid;//该报文是否有效
    unsigned char        m_bUpdated;    //是否更新了
    unsigned char        m_CAN_Port;//报文广播端口
    unsigned char        m_FrameType;//扩展帧还是标准帧
    unsigned int         m_U32ID;//报文ID
    unsigned int         m_Period;//报文广播周期
    unsigned char        m_Endian;   //大小端结构
    unsigned char        m_DLC;  //该报文长度
    unsigned char        m_DataGroup1;//数据1的DataGroup号
    unsigned char        m_Chan1;//数据1在该group中的位置
    unsigned char        m_DataGroup2; //数据2的DataGroup号
    unsigned char        m_Chan2; //数据2在该group中的位置
    MsTime               m_CurrentTick;//记录周期时间
} St_CANAUX_Broadcast_MSG_Info;
typedef struct
{
    unsigned char                   m_Total_Message;    //该unit下的广播报文总数
    St_CANAUX_Broadcast_MSG_Info    m_AuxBroadcast_Message[48];//48条报文信息
} St_CANAUX_Broadcast_Info;
typedef struct
{
    float               m_fSecondaryVoltage;
    float               m_fChannelVoltage;
    float               m_fDCVoltageOffset;
    BYTE                m_btDspBoardType;
	BYTE                m_btSw_Freq;                       
    BYTE                m_btHZ_XMFR; 
    BYTE                m_btGrid_Voltage;
	BYTE               m_btDSPBoardType;
	WORD               m_wDSP_Version:14;//YYXX 1901,19年第1版
	WORD               m_wDSP_TYorZY:2;//00:TY,01:ZY	  	
    BYTE                m_btReadResult;//0:false,1,true.
} DSP_Basic_Parameter;
typedef struct
{
    float               m_fV_KP;
    float               m_fV_KI;
    float               m_fI_KP;
    float               m_fI_KI;
    BYTE                m_btReadResult;//0:false,1,true.
} DSP_Adjust_Parameter;
typedef struct
{
    BYTE               m_DSP_Indentity;
	  BYTE               m_DSP_SlaveCountOrMasterID;
	  BYTE               m_DSP_SlaveID[8];
    BYTE               m_btReadResult;//0:false,1,true.
} DSP_Indentity_Parameter;
typedef struct
{
    unsigned char m_IV_ID;
    unsigned char m_M0UnitCnt;
    unsigned char m_M0UnitIdx;
    E_Para_AckStatus CmdStatus;
    unsigned short SN;
    unsigned char m_ReSendCnt;
    MsTime     m_OverTime;
    unsigned char m_M0ID[4];   //暂时最大支持一拖四， 一个IV 对应4台电源
    E_Para_AckStatus m_M0Status[4];

    MP_CAN_MSG m_Msg;
} St_PowerAdjAckMgr;

class   CANBusParse
{
public:
    //function
    static void Init(void);
    static void DoWork();
    static void CANParse(unsigned char CANx);
    static unsigned char Aux_Confirm_Ack(const MP_CAN_MSG * RecvMsg,unsigned char uc_errorcode=0);
    static unsigned short Send_CAN_Cmd(MP_CAN_MSG* CAN_msg);
    //members
	static MsTime lastAuxTriggerTime[MAXCHANNELNO];
    static  UInt16        Priority ;
    static  UInt16        TaskID ;
    static ST_CAN_SN      st_CAN_SN;
    static unsigned short StepID[MAXCHANNELNO];
// 	static MsTime         SyncAuxTime;
    static unsigned char DIDO_Ctrl; // 0:不存在DIDO板， 1：MultiFunc Board 上的DIDO； 2： 新的32通道DIDO板；
    static  unsigned char   AuxInfoShiftBit[MAXCHANNELNO];
		static MP_CAN_MSG 		 CAN_ParseRxMsg;
    static unsigned int INFU_Error;
//#if(SUPPORT_AUX==1)
// 	static St_TimeDiff       SyncTimediff;//debug time sync
#if(USE_AS_AUXMCU == 0 )
    static ST_IVAskTable   st_IV2AuxInfoMgr[MAXCHANNELNO][MAXAUXUNIT];
    static St_SNTable      st_SNResTable[MAXCHANNELNO][MAXSNTYPE];   //limit SN号检查
    static St_SNTable      st_SNCheckTab[MAXCHANNELNO][MAXSNTYPE];   //limit SN号检查
    static St_AuxInfoRes   st_AuxInfoRes[MAXCHANNELNO];
    static St_ClrLimitAck  st_ClrLimitAckMgr[MAXCHANNELNO];// clear limit ack
    static float           f_DeltaValueTime_back[MAXCHANNELNO];
    static UWord32         uw32_MaxTime_Para_UpdateOut;
#else
    static St_AuxStatusRpt  st_AuxRptInfoMgr[MAX_IVMCU_PER_SYS];
    static unsigned char AuxBrdCstReciveIdex ;
#endif
//#endif

    //#if(SUPPORT_CAN_BMS==1)
public:
    static bool  CANBMS_bWork; // zyx 180518
    static bool  CANBMS_bMap; //zyx 181010 make sure can Map was init.
    static St_CANBMS_ConfigBuf    m_CANConfig;
    static St_CANConfigAssignTab  m_CANAssignMD5[MAXCHANNELNO];
    static St_BMSUnsafeInfo   		m_BMS_OverTime[MAXCHANNELNO];  //
    static St_CANBMS_SendBuf  		m_BMS_SendBuf[MAXCHANNELNO];//sch& Dilaog send buf
    static St_BMS_SendUnit        m_BMS_BroadCstBuf[MAXCHANNELNO][BMS_BROAD_CST_COUNT];  //广播
    static St_BMS_AckConfirmMgr   m_BMSAckConfirmMgr[MAXCHANNELNO];
    static St_BMS_SendInfo        m_BMS_SendInfoMgr[MAXCHANNELNO];
    static St_CANBMS_AuxInfo      m_CANBMS_AuxInfo;
    static ST_AuxMultiMsgManage   m_BMS_AuxMultiMsgManage;//170828 zyx
    static unsigned char          m_BMS_SendBuf_RunOn[MAXCHANNELNO];  //IV MCU Send Buf Run Enable
    static unsigned char m_BroadCstIdex[MAXCHANNELNO];
    static St_CANAUX_Broadcast_Info m_CANAUX_Broadcast;
    static void CANBMS_BroadcastAuxMsg(void);
    //#endif
    static bool 				m_SubACKUnitFlg[MAXMCUUNIT];
    //can bms
    //#if(SUPPORT_CAN_BMS == 1)
	static bool lastAuxTrigger[MAXCHANNELNO];
	static bool lastCANBMSData[MAXCHANNELNO];
public:
    static void CANBMS_IVDoWork();
    static void CANBMS_AuxDoWork();
    static bool ForwardFDCANRxData(ST_CAN_RxMSG OrgMsg);
    static bool CANBMS_ProcessOrigMsg(unsigned char CANx,ST_CAN_RxMSG OrgMsg);
    static void CANBMS_UpdateBrdCstMsg(MP_CAN_MSG *Parse_Msg);
    static bool Assign_CANConfig(unsigned char uc_ChannelNo);
    static bool Clear_CANConfig(unsigned char uc_ChannelNo);  //Add by DKQ 01.04.2015
    static MetaItemsBufAddr GetMetaItemsBufAddr(int cfgIdx, unsigned char Interval);

    static uint32 CAN_Inverter_Fuse_Error(unsigned char CANx,const MP_CAN_MSG * Parse_Msg);
    static void CANBMS_AuxAssembleMsgs(unsigned char CANx,const ST_CAN_RxMSG * can_msg);
    static void CANBMS_IVAssembleMsgs(unsigned char CANx,const ST_CAN_RxMSG * can_msg);
    static MP_MCU_MSG CanMsg2McuMsg(const MP_CAN_MSG CAN_msg);

private:
    static void CANBMS_AuxParseMsg(unsigned char CANx,const MP_CAN_MSG * Parse_Msg);
    static void CANBMS_IVParseMsg(unsigned char CANx,const MP_CAN_MSG * Parse_Msg);

#if(USE_AS_AUXMCU == 0)	  //IV Unit 	
public:  //interface
    static void CANBMS_Split2SendMsg(unsigned char IV_ChanNo,const MP_CAN_MSG *msg,St_BMS_SendInfo SendInfo);//slipt can msg
    static unsigned char CANBMS_GetChn(unsigned char AuxID);
    //static void CANBMS_LogData(unsigned char uc_ChannelNo);
// 	static void CANBMS_QuickSortCfg(unsigned char SignalCnt,St_CANBMS_ParsedMeta CAN_Signal[]);
    static void CANBMS_SetBaudRate(unsigned char uc_ChannelNo);
    static void CANBMS_ClearBMS(unsigned char uc_ChannelNo); //Add DKQ 01.04.2015
    static void CANBMS_StopAllSend(unsigned char uc_ChannelNo);
    static void CANBMS_setBroadcastWithAuxBorad(unsigned char uc_ChannelNo);
	static signed long GetOutboundMessageIndex(unsigned char ChnNo,unsigned long MsgID);          //190617,hpq
	static signed char OutboundMsgPack(unsigned char ChnNo,unsigned long MsgInfoIdx,void *data);  //190617,hpq
    static void CANOutboundSignalLinearTransformation(St_CANBMS_Config * p_CAN_Cfg,int msginfoidx);

    static void SetCanBmsData(MP_SET_CANMSG *pCmd);
    //	 static bool CANBMS_InitConfirmBuf(void);
    static void CANBMS_DoAckCheck(unsigned char IV_ChnNo);
    static bool CANBMS_addToConfirm(unsigned char IV_ChnNo,MP_CAN_MSG *FirstMsg,MP_CAN_MSG *SecondMsg);
    static void CheckCanBmsMap(void);
    static unsigned char CheckCAN_Port_Property(unsigned char CANx);
private:
    static void CANBMS_DoAckProcess(MP_CAN_MSG *ackmsg);
    static void CANBMS_IV_SENDBMS();

    static int CANBMS_binarySearchCfg(unsigned char cfgidx,unsigned int searchID, CANBMS_METAIdx high, CANBMS_METAIdx low); //170606 zyx
//	static void CANBMS_MsgSendWithAuxBorad(unsigned char uc_ChannelNo);
//  static void CANBMS_MsgSendWithBuiltInCAN(unsigned char uc_ChannelNo);
    static void CANMBMS_SendMsg(unsigned char uc_ChannelNo);
    static void CANBMS_setBroadcastWithBuiltInCAN(unsigned char uc_ChannelNo);
    static void CANBMS_IVAssembleBuiltInMsgs(unsigned char CANx,unsigned char Chan,const ST_CAN_RxMSG * OrgMsg);
    static bool CANBMS_IsUseBuiltInCAN(unsigned char uc_ChannelNo);
    static unsigned char CANBMS_GetBuiltInCanPort(unsigned char uc_ChannelNo);
//   static unsigned char CANBMS_GetCANBMSChn(unsigned char uc_ChannelNo);
    static unsigned char CANBMS_GetChanOfCANx(unsigned char CANx);
//Built_in CANBMS  End//

    static Un_Data CANBMS_endianParse(const Un_Data* Data,unsigned char cfgidx,unsigned short uc_SignalIdx);
#else   //Aux 
private:
    static void CANBMS_ConfirmAck(const MP_CAN_MSG *ackmsg);
    static void CANBMS_ForwardMsg(const ST_CAN_RxMSG * OrgMsg);
    static MP_CAN_MSG ToNormalCanMsg( ST_SEND_MULTI_CANBMS_MSG m_MultiCanMsg ); //170828 zyx
    static void CANBMS_ForwardMsg(unsigned char CANx, const ST_CAN_RxMSG * OrgMsg);
    static void CANBMS_TrySwitchBuffer(void);
    static void CANBMS_ForwardMsg2(unsigned char CANx);

#endif

    //#endif //end of CAN BMS

//#if(SUPPORT_AUX == 1)
    static void Aux_CmdParse(MP_CAN_MSG * CAN_Msg);
    static unsigned short Send_Aux_Cmd(MP_CAN_MSG* CAN_msg);
// 	static void AddToConfirmBuf(unsigned char CANx,const MP_CAN_MSG * can_msg); //temp
#if(USE_AS_AUXMCU == 1)
public:
    //Aux MCU Send Command (or Report Msg to IV MCU)
    static void Aux_RptLimitStatus(ST_AUX_LIMIT *Limit,float fValue);
    static void Aux_RptAnyLimitStatus(ST_AUX_IFANY_LIMIT *Limit,float fValue);
    static void Aux_RptData(ST_AUX_DATAPOINT Aux_DataPoint);
    static void Aux_RptUnsafe(unsigned char IVUnit,unsigned char IVChn,unsigned char AuxChn,unsigned char AuxType,unsigned char UnsafeStatus,float f_Value);
    static void Aux_RptMTCTempUnsafe(unsigned char IVUnit,unsigned char IVChn,unsigned char AuxChn,unsigned char AuxType,float f_Value, unsigned short SafetyGotoTag);
    static void Aux_RptMTCFanUnsafe(unsigned char IVUnit,unsigned char IVChn,unsigned char AuxChn,unsigned char AuxType);
// 	static void Aux_RequestTime();
//    static void Aux_Sync_Time(MsTime SyncTime,unsigned char SyncStatus);
    static void Aux_CANStatusRpt();
    static void Aux_UpdateStatus(MP_CAN_MSG *Msg);
    static void Aux_RptMetaData(unsigned char AuxChn,unsigned char MV_Type,float f_Value);
    //static void Report_Data_Ack(unsigned char AuxChnNo,unsigned char Auxtype,unsigned char VirtualID,float Data);

private:  //aux Mcu Cmd receive and parse
    static bool Aux_AddLimitToBank(MP_CAN_MSG *Msg); //从CANBus上获取下载的limit并加到limitbankone中
    static void Aux_ClearIVsLimit(MP_CAN_MSG *Msg);
//     static void Aux_DownloadSdlPara(MP_CAN_MSG *Msg);
//     static void Aux_SdlEnable(MP_CAN_MSG *Msg);
    static void Aux_AddSafetyCfg(MP_CAN_MSG *Msg);
    static void Aux_ClearSafetyCfg(MP_CAN_MSG *Msg);
    static void Aux_RxSetCtrl(MP_CAN_MSG *Msg);
    static void Aux_RxCtrlRunON(MP_CAN_MSG *Msg);
    static void Aux_RxSetMTCSafetyCheck(MP_CAN_MSG *Msg);  //add by zc for MTC 07.17.2018
    static void Aux_RxClearMTCSafetyCheck(MP_CAN_MSG *Msg);  //add by zc for MTC 08.20.2018
    static void Aux_RxDOCtrl(MP_CAN_MSG *Msg);
    static void Aux_RxAOCtrl(MP_CAN_MSG *Msg);
    static void Aux_RxTempChamberCtrl(MP_CAN_MSG *Msg);
    static void Aux_RxTEC_ChamberCtrl( MP_CAN_MSG *Msg);  //add by ZC 01.02.2017
    static void Aux_RxMTCCtrl(MP_CAN_MSG *Msg);  //add by ZC 04.24.2018
    static void  Aux_RxExtChargeCtrl(MP_CAN_MSG *Msg);
    static void Aux_RequestDataProc( MP_CAN_MSG *Msg);
    static void Aux_ClearRequestData(MP_CAN_MSG *Msg);
//     static void Aux_TrigLogProc( MP_CAN_MSG *Msg);
    static void Aux_DownloadScheduleMap(MP_CAN_MSG *Msg);
    static void Aux_SysCtrlAlarm(unsigned char CANx, MP_CAN_MSG *Msg);
    static void Aux_TrigGroupLogProc_DataPack(MP_CAN_MSG *Msg,unsigned char uc_lastPoint);
	static void Aux_TrigGroupLogProc(MP_CAN_MSG *Msg);
	static void Aux_TrigGroupLogProcLast(MP_CAN_MSG *Msg);

#else	   //IV MCU	
    //IV SEND Command
public:
    static int IV_SendSchMapToAUX(unsigned char uc_channelNo, unsigned char Auxtype);
    static int  IV_SendSafetyInfoToAUX(unsigned char uc_channelNo,unsigned char Auxtype,float fHigh,float fLow,unsigned char uc_AuxSafe_Type);   //uc_SpecialPara=0: 常规的； 1：发送SafetyVoltage; 2:发送SafetyCurrent；3：发送SafetyTemperature。
    static int  IV_ClearAuxSafetyInfo(unsigned char uc_channelNo,unsigned char Auxtype,unsigned char VirtualID,unsigned char Type);
    static int  IV_DownloadSDLToAUX(unsigned char uc_channelNo,unsigned char Auxtype,unsigned char Type,float SDLFACTOR,unsigned char DestId,unsigned char UnitId);
    static void IV_DownloadLimitToAUX(unsigned char uc_channelNo,unsigned short u16_StepID,unsigned char LimitIdx);//,unsigned char VirtualID);
    static int  IV_ClearLimitsToAUX(unsigned char uc_channelNo,unsigned char Clear4New,unsigned short StepID);
    static int  IV_RequestDataToAUX(ST_REQUEST_AUXDATA Set_DataRpt);
    static int  IV_TriggerLogToAUX(unsigned char uc_channelNo);
	static int IV_TriggerLastLogToAux(unsigned char uc_channelNo);
//     static int  IV_EnableSDLToAUX(unsigned char uc_channelNo,unsigned short u16_StepID,unsigned char bEnable);
// 	static void IV_TimeSyncToAux(MsTime *SyncTime,unsigned char RequestType,unsigned char CanPort);
    static void IV_SendDigitOut(unsigned char uc_channelNo,unsigned char bSet);
    static void IV_SendAnalogSingnalOut(unsigned char uc_channelNo,unsigned char bSet);
    static void IV_SendTempChamberOut(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType,float SetValue);
    static void IV_SendTempChamberOutRunON(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType, unsigned char  RunON);
    static void IV_SendMTCTemperatureSaftyCheckOFF(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType);
    static void IV_SendMTCSafetyTimeOut(unsigned char uc_channelNo,unsigned short  AuxCh,unsigned char  AuxType,unsigned short MTCSafetyTimeValue, unsigned char SafetyGotoTag); //07.13.2018 add by zc for MTC
    static void IV_SendExtChargeOut(unsigned char uc_channelNo,unsigned char bSet);

    //static void IV_ClearLimitsToAUX_Retransmission(unsigned char uc_channelNo,unsigned char Clear4New,unsigned short StepID);
    static void IV_DownloadLimitToAUX_Retransmission(unsigned char uc_channelNo,unsigned short u16_StepID,unsigned char LimitIdx);
    //IV Receive Command
private:
    static int  IV_RxLimitData(MP_CAN_MSG *Msg);
    static int  IV_AuxUnsafeRpt(MP_CAN_MSG *Msg);
    static int  IV_AuxMTCTempUnsafeRpt(MP_CAN_MSG *Msg);
    static int  IV_AuxMTCFanUnsafeRpt(MP_CAN_MSG *Msg);
    static int  IV_AuxLimitRpt(MP_CAN_MSG *Msg);
    static int  IV_ProcessAuxStatusInfo(MP_CAN_MSG *Msg);
    static int  IV_AckProcess(MP_CAN_MSG *Msg);
    static int IV_RxMetaVariable(MP_CAN_MSG *Msg);
    // Aux Status and SN Check management
public:
    static void IV_InitStatusMgr();
    static int  IV_IfAuxNormal(unsigned char uc_ChannelNo);
    static bool IV_CheckAuxSNRes(unsigned char uc_ChannelNo,unsigned char SNtype);
    static bool IV_ClrAuxLimitRes(unsigned char uc_ChannelNo);
    static void IV_ClearAuxSNList(unsigned char uc_ChannelNo,unsigned char SNtype);
private:
    static void IV_DoAuxStatusCheck(unsigned char IV_Idx);
    static void IV_CheckAuxCount(unsigned char IV_Idx,unsigned char AuxUint);
    static void IV_CheckCANConnected(unsigned char IV_Idx,unsigned char AuxUint);
    //	static void IV_LaunchStatusAsk(unsigned char IV_ChnIdx,unsigned char AuxUint,unsigned short StepID);

#endif
//#endif //SUPPORT AUX

    //------------------- INTER UNIT PARALLEL GROUP CAN  -----------------------
#if(USE_AS_AUXMCU == 0)
public:
		static MP_CAN_MSG		 AssembleMsg;
    const static  unsigned int Parallel_BroadcastID;
    static St_ParaAckMgr m_Para_MasterAckMgr[CAN_CMD_PARA_MAX][PARA_MAX_BUF]; //master unit
    static unsigned char m_ParaBufIdx[CAN_CMD_PARA_MAX];
    static St_SubMaster_Confirm m_SubMasterAckMgr[3];	  //submaster unit
    static MsTime         m_ParaDataUpdateTick;
    //end.
    static unsigned short Send_Para_Cmd(MP_CAN_MSG* CAN_msg);
    static void Send_SYSCtrl_Cmd(MP_CAN_MSG* CAN_msg);
    static St_PowerAdjAckMgr m_Para_PowerAdjAckMgr;
    // 	//-------------- PARSE -----------------------------
private:
    static unsigned char beSubMasterAckUnitID(unsigned char uc_UnitID);
    static void Parallel_CmdParse(MP_CAN_MSG * CAN_Msg);
    static unsigned char Parse_Parallel_Master_RequestStart(MP_CAN_MSG *Msg); //CAN_CMD_PARA_START:  //master --> submaster: start
    static void Parse_Parallel_Master_SetVolClamp(MP_CAN_MSG *Msg);
//	static void Parse_Parallel_Master_SetCurrentSimulationVolClamp(MP_CAN_MSG *Msg);//09.25.2017 add by zc
//	static void Parse_Parallel_Master_SetPowerSimulationVolClamp(MP_CAN_MSG *Msg);//10.04.2017 add by zc
    static void Parse_Parallel_Master_RequestStop(MP_CAN_MSG *Msg);//CAN_CMD_PARA_MASTER_STOP:  //master --> submaster: stop
    static void Parse_Parallel_Master_RequestStepChange(MP_CAN_MSG *Msg);//CAN_CMD_PARA_MASTER_STEPCHANGE:  //master --> submaster: jump step
    static void Parse_Parallel_Master_RequestUpdateOutput(MP_CAN_MSG *Msg);//CAN_CMD_PARA_MASTER_UPDATEOUTPUT:  //master --> submaster: update control value
// 	static void Parse_Parallel_Master_RequestData(MP_CAN_MSG *Msg);// CAN_CMD_PARA_REQUESTDATA:    //master --> submaster:  request data from submaster
    static void Parse_Parallel_Submaster_ReportData(MP_CAN_MSG *Msg);//CAN_CMD_PARA_SUB_REPORTDATA:   //submaster --> master: report data (SDL, resolution current&voltage pair)
    static void Parse_Parallel_Submaster_ReportUnsafe(MP_CAN_MSG *Msg);//CAN_CMD_PARA_SUB_UNSAFE:   //submaster --> master: report unsafe
    static void Parse_Parallel_Submaster_ReportIrregulation(MP_CAN_MSG *Msg); //CAN_CMD_PARA_SUB_IRREGULAR //submaster --> master: report irregulation
// 	static void Parse_Parallel_Submaster_ReportCtrlError(MP_CAN_MSG *Msg) ; //CAN_CMD_PARA_SUB_CTRLERROR //submaster --> master: report ctrl value problem
    // 	static void Parse_Parallel_Submaster_ReportCurrentAbnormal(MP_CAN_MSG *Msg); //CAN_CMD_PARA_SUB_CURR_ABN:   //submaster --> master: report current abnormal
    // 	static void Parse_Parallel_Submaster_ReportVoltageAbnormal(MP_CAN_MSG *Msg);// CAN_CMD_PARA_SUB_VOLT_ABN:   //submaster --> master: report voltage has big different in sub-unit's parallelled channels
    static void Parse_Parallel_Master_SetFilter(MP_CAN_MSG *Msg);
    //-------- CALL FOR SENDING CAN ----------------------------------
public:
    static void Parallel_MasterSend_Start(bool bNeedfilter,unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range);
    static void Parallel_MasterSend_RUN(unsigned char SubUnitID,bool bNeedfilter, unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range);
    static void Parallel_MasterSend_Clamp_Volt(unsigned char uc_ChannelNo);
//	static void Parallel_MasterSend_CurrentSimulation_Clamp_Volt(void); // 09.25.2017 add by zc
//	static void Parallel_MasterSend_PowerSimulation_Clamp_Volt(void); // 10.04.2017 add by zc
    static void Parallel_MasterSend_StepChange(bool bNeedfilter,unsigned char ucCtrlType, float fCtrlValue, unsigned char uc_Range);
    static void Parallel_MasterSend_UpdateOutput(float fCtrlValue,bool IsKeyPiont);
    static void Parallel_MasterSend_Stop();
    static void Parallel_MasterSend_DoStop(unsigned char SubUnitID);
    static void Parallel_MasterSend_SetFilter(unsigned char bIfFilter,unsigned char bIfCCCV_Ramp);
    static void Parallel_Series2BoosterSend_OutVoltage(float fVoltageValue);
    static void Parallel_SubmasterSend_ReportData(float fCurrentValue, float fVoltageValue);
    static void Parallel_SubmasterSend_ReportUnsafe(unsigned int uint32_UnsafeType, float fValue,unsigned char uChanIdx);
    static void Parallel_SubmasterSend_ReportIrregulation(unsigned short uChanIdx, unsigned short uType);
// 	static void Parallel_SubmasterSend_ReportCtrlProblem(unsigned short uChanIdx, unsigned short uType);
    static void Parallel_SubmasterSend_ReportCurrAbnormal(float fValue);
    static void Parse_Parallel_Sub_RequestRun(MP_CAN_MSG *Msg);
    static void Parse_Parallel_Sub_RequestStop(MP_CAN_MSG *Msg);
    static void Parse_Parallel_Master_Request_Series2Booster_OutVoltage(MP_CAN_MSG *Msg);

    static void Parallel_DoMasterAckCheck();
    static bool Parallel_ConfirmAck( const MP_CAN_MSG * RecvMsg, unsigned char uc_errorcode);
    static void Parallel_AckProcess(MP_CAN_MSG* CAN_msg);
    static bool Parallel_CheckAck(unsigned char Cmd,unsigned char BufIdx);
    static bool Parallel_SubMasterConfirm(const MP_CAN_MSG * RecvMsg);
    //	static void Parallel_ResetSubAckInfo();
    static bool SubMasterIfFilter(unsigned short ChanIdx,unsigned short ControlType);
    static void DSP_IV_SendBasicParameterToDsp(DSP_Basic_Parameter * DspBasicPara,unsigned char DSP_ID);
    static void DSP_IV_SendAdjustParameterToDsp(DSP_Adjust_Parameter * DspAdjustPara,unsigned char DSP_ID);
    static void DSP_IV_ReadBasicParameterFromDsp(unsigned char DSP_ID);
    static void DSP_IV_ReadAdjustParameterFromDsp(unsigned char DSP_ID);
		static void DSP_IV_SendIndentityParameterToDsp(DSP_Indentity_Parameter * DspIndentityPara,unsigned char DSP_ID);
		static void DSP_IV_ReadIndentityParameterFromDsp(unsigned char DSP_ID);
    static void DSP_Feedback_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg);
    static unsigned short CANCalCRC16(unsigned char *ptr,unsigned char count);
    static void IV_SendTOSysCtrl_Abnormal(void);
    static void IV_SendTOSysCtrl_BackToNormal(void);
    static void IV_SendTOSysCtrl_RUN(void);
    static void IV_SendTOSysCtrl_STOP(void);
#endif
public:
#if(USE_AS_AUXMCU == 0)
// 	static void Send_M0_RefreshData(float Vpp, float Vmm);
    static void Send_M0_COMMON2Data(unsigned char m_com,St_PowerData m_data);
//   static void Send_M0_Config(unsigned char m_com,unsigned char m_Unit);
    static void Send_M0_WatchDog(unsigned char m_SN);
    static bool PowerAdj_CheckAck(void);
    static void PowerAdj_DoAckCheck(void);
    static void PowerAdj_AckProcess(MP_CAN_MSG* CAN_msg);
private:
    static void Power_M0_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg);
#else
private:
    static void Power_AUX_CmdParse(unsigned char CANx, MP_CAN_MSG * CAN_Msg);
public:
    static void Send_AUXToIV_COMMON2Data(unsigned char m_com,ST_M4_Value m_data);//
#endif
};

#endif
