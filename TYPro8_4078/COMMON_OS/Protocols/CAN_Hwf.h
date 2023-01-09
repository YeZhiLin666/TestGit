#ifndef   __CANPROTOCOL_H
#define   __CANPROTOCOL_H
//CAN协议头文件

#define   CAN_RX_BUF_LENGTH          3000  // 4000  // 20190917 没SDRAM 32M空间了,借用一些。
#define   CAN_TX_BUF_LENGTH          3000 // 4000  // 20190917 没SDRAM 32M空间了,借用一些。

//CAN BMS
#define   CAN_MULTI_PACK_BUF_LEN     254
#define   SEND_CAN_BMS_MAX_LEN       40
#define   SN_LENGTH                 IV_MAX_LIMIT_NUMBER*2
#define   MAXIFANYLIMIT             IV_MAX_LIMIT_NUMBER*2
#define   MAXSNTYPE                 2
#define   MAXUNITID                 255
#define   SYSTEMCTRLBOARD_ID				255
///////////CAN command/////////////////////

enum CAN_CMD_TYPE
{
    Parallel_CMD_TYPE = 0,
    AUX_CMD_TYPE = 1,
    BMS_CMD_TYPE = 2,
    PARAL_WATCHDOG=3,
    AUX_WATCHDOG=4,
    DSP_PARA_SET=5,
    Power_M0_ADJ =6,
    SYSTEMCTRL_SET=7,

};
//////CMD SIZE<<0 - 63>> /////
enum  AUX_CMD
{
    //IV发送
    CAN_CMD_DOWNLOAD_LIMITS    =  1,             //HIGH Priority
    CAN_CMD_DOWNLOAD_SDL       =  2,
    CAN_CMD_DOWNLOAD_SAFETY    =  3,
    CAN_CMD_CLEAR_LIMITS       =  4,
    CAN_CMD_CLEAR_SAFETY       =  5,
    CAN_CMD_REQUEST_DATA       =  6,
    CAN_CMD_CLEAR_REQUEST_DATA =  7,
    CAN_CMD_ENABLE_SDL         =  8,
    CAN_CMD_TRIGGER_LOG        =  9,           //HIGH Priority
    CAN_CMD_ASK_STATUS         =  10,
    CAN_CMD_TIME_SYNC          =  11,      //
    CAN_CMD_SET_CTRL           =  12,       // CAN_CMD_SET_DOCTRL         = 12,
    CAN_CMD_CTRL_RUNON         =  13,     //温控箱是否与SCH同步有关，RunON为0时，关闭温控箱
    //CAN_CMD_SET_TEMPCTRL       = 13,
    CAN_CMD_AIO_CTRL            =14,
    CAN_CMD_MTC_SAFETY_CHECK	 =  15,   //设置MTC温度保护  //07.13.2018 add by zc for MTC
    CAN_CMD_MTC_SAFETY_STOP 	 =  16,   //设置MTC温度保护  //08.20.2018 add by zc for MTC
    CAN_CMD_DOWNLOAD_SCHMAP    = 17,
	CAN_CMD_TRIGGER_GROUPLOG = 18, //WL 12.18.2019
	CAN_CMD_TRIGGER_GROUPLOG_LAST = 19, //WL

    //AUX 发送
    CAN_CMD_CONFIRM_ACK         =  20,
    CAN_WATCHDOG       		 =  25,         //接收发送一致
    CAN_CMD_REQUEST_DATA_ACK    =  21,
    CAN_CMD_REPORT_UNSAFE       =  22,
    CAN_CMD_REPORT_LIMIT_STATUS =  23,
    CAN_CMD_REQUEST_TIME        =  24,     //
    CAN_CMD_STATUS_RPT          =  27,
    CAN_CMD_REPORT_MV           =  26,
    CAN_CMD_REPORT_MTC_TEMP_UNSAFE =  28,  // add by zc 08.08.2018 for MTC
    CAN_CMD_REPORT_MTC_FAN_UNSAFE =  29,  // add by zc 08.31.2018 for MTC
};
enum  Power_M0_CMD
{
//     CAN_CMD_M0_START  = 1,
    CAN_CMD_M0_REQUIRE=2,
    CAN_CMD_M0_REQUIREACK=0xA2,
    CAM_CMD_M0_REPORT,
// 	CAN_CMD_M0_STOP ,
// 	CAN_CMD_M0_REFRESHDATA,
// 	CAN_CMD_M0_RESTART,
// 	CAN_CMD_M0_UNSAFE,
// 	CAN_CMD_M0_Config,
// 	CAN_CMD_M0_START_ACK = 0xA1,
// 	CAN_CMD_M0_STOP_ACK = 0xA2,
// 	CAN_CMD_M0_REFRESHDATA_ACK = 0XA3,
// 	CAN_CMD_M0_RESTART_ACK = 0xA4,
// 	CAN_CMD_M0_Config_ACK = 0xA6,
};
enum  PARAL_CMD
{
    CAN_CMD_PARA_START = 1,
    CAN_CMD_PARA_MASTER_STOP = 2,
    CAN_CMD_PARA_MASTER_STEPCHANGE = 3,
    CAN_CMD_PARA_MASTER_UPDATEOUTPUT= 4,
    CAN_CMD_PARA_REQUESTDATA = 5,
    CAN_CMD_PARA_SUB_REPORTDATA = 6,
    CAN_CMD_PARA_SUB_UNSAFE = 7,
    CAN_CMD_PARA_SUB_CURR_ABN = 8,
    CAN_CMD_PARA_SUB_VOLT_ABN = 9,
    CAN_CMD_PARA_SUB_IRREGULAR = 10,
    CAN_CMD_PARA_SUB_CTRLERROR = 11,
    CAN_CMD_PARA_ACK = 12,
    CAN_CMD_PARA_SETCLAMP = 13,
    CAN_CMD_PARA_SUB_RUN=14,
    CAN_CMD_PARA_SUB_STOP=15,
    CAN_CMD_PARA_SET_ISIMUCLAMP = 16,  //09.25.2017 add by zc
    CAN_CMD_PARA_SET_PSIMUCLAMP = 17,  //10.04.2017 add by zc
    CAN_CMD_PARA_SERIES2BOOSTER_REPORT_VOLTAGE = 18,
	CAN_CMD_PARA_MASTER_SET_FILTER = 19,
    //记录最大值
    CAN_CMD_PARA_MAX = 20,  //保持最大
};
enum BMS_CMD
{
    CAN_CMD_TRANS_FIRST_PACK   = 1,
    CAN_CMD_TRANS_SECOND_PACK  = 2,
    CAN_CMD_TRANS_THRID_PACK   = 3,
    CAN_CMD_SET_BMS_BAUDRATE   = 4,
    CAN_CMD_SET_BROAD_CAST     = 5,
    CAN_CMD_STOP_ALL_MSG       = 6,
    CAN_CMD_CLEAR_BMS          = 7,
    CAN_CMD_BMS_MSG            = 8,
    CAN_CMD_BMS_ACK = 10,
};
enum DSP_CMD
{
    CAN_CMD_DSP_SET_BASIC_FIRST_PACK   = 1,
    CAN_CMD_DSP_SET_BASIC_SECOND_PACK  = 2,
    CAN_CMD_DSP_SET_ADJUST_FIRST_PACK   = 3,
    CAN_CMD_DSP_SET_ADJUST_SECOND_PACK   = 4,
    CAN_CMD_DSP_READ_BASIC_FIRST_PACK   = 5,
    CAN_CMD_DSP_READ_BASIC_SECOND_PACK  = 6,
    CAN_CMD_DSP_READ_ADJUST_FIRST_PACK   = 7,
    CAN_CMD_DSP_READ_ADJUST_SECOND_PACK   = 8,
    CAN_CMD_DSP_SEND_ERROR_PACK   = 9,
	  CAN_CMD_DSP_SET_INDENTITY_FIRST_PACK   = 13,
    CAN_CMD_DSP_SET_INDENTITY_SECOND_PACK   = 14,
	  CAN_CMD_DSP_READ_INDENTITY_FIRST_PACK   = 15,
    CAN_CMD_DSP_READ_INDENTITY_SECOND_PACK  = 16,

};

enum SYSTEM_CMD
{
    CAN_CMD_SYSTEM_ABNORMAL   =15,
    CAN_CMD_SYSTEM_BACKTONORMAL  =16,
    CAN_CMD_SYSTEM_RUN=17,
    CAN_CMD_SYSTEM_STOP=18,
};
enum CAN_ERROR_CODE
{
    NO_ERROR,
    LIMIT_BANK_FULL,
    IFANY_LIMIT_BANK_FULL,
    ALLBIT_ERROR,
    AUX_TYPE_OVERFLOW,
    IV_CHNO_OVERFLOW,
    AUX_CHNO_OVERFLOW,
    IV_UNIT_ID_VOERFLOW,
    UNIT_ID_NOT_FIND,
    AUX_DATA_TYPE_OVERFLOW,
    AUX_DATA_REQUEST_BANK_FULL,
    AUX_LIMIT_BANK_FULL,
    AUX_IFANY_LIMIT_BANK_FULL,
    AUX_SAFETY_BANK_FULL,
};

enum CAN_BUFTXSTATUS
{
    CAN_NULL = 0,
    CAN_SEND_READY = 1,
    CAN_SENDED = 2,
    CAN_WAITCONFIRM = 3,
    CAN_CONFIRMED = 4,

};

//aux 数据类型
enum
{   //数据类型，原始数据0， 一阶时间导数 1， 二阶时间导数2
    ORIG_DATA,
    DX_DT_DATA,
    D2X_DT2_DATA,
    DATA_TYPE_ERROR,
};
/************************************************************************/
/*                               发送命令结构                         */
/************************************************************************/
#pragma pack(1)

enum
{
    CAN_PRIO_HIGH = 0,
    CAN_PRIO_LOW = 1,
    //CAN_PRIO_HIGHER = 1,
    //CAN_PRIO_MID = 2,
    //CAN_PRIO_LOW = 3,
};
typedef struct
{
    unsigned int    Cmd:8;
    unsigned int    CmdType:4;    //这里我们用于区别命令
    //unsigned int    AuxChnType:4;	//
    unsigned int    SourceID:8;
    unsigned int    DestID:8;
// 	unsigned int    Priority:1;
    unsigned int    Rsvd:4;
} ST_ArbinCAN_MsgID ;               //4BYTE

typedef union
{
    ST_ArbinCAN_MsgID    m_CANID;
    unsigned int         m_U32ID;
} UN_CANID;

typedef union
{
    struct
    {
        unsigned short IV_ChnNo:4;
        unsigned short SN:10;
    } SN_Bit;
    unsigned short Uint16SN;
} UN_st_CAN_SN;
//	typedef union
//	{
//		unsigned char    SendStatus;    //CAN发送状态
//	   unsigned char    ParsedStatus;     //被解析的状态（parss函数）
//	}ST_CAN_STATUS;

#pragma pack()


//#if(SUPPORT_AUX == 1)

enum
{
    AUX_UNSAFE_TOIV,
    AUX_SAFE,
    OnlineRSMVol_UNSAFE,
    OnlineRSMCur_UNSAFE,
    OnlineRSMTemp_UNSAFE,
};    //DKQ 12.17.2013

enum AUX_SAFETY_SET
{
    AUX_SAFETY_LOW_END,
    AUX_SAFETY_HIGH_END,
    AUX_SAFETY_Voltage_LOW,
    AUX_SAFETY_Voltage_HIGH,
    AUX_SAFETY_Current_LOW,
    AUX_SAFETY_Current_HIGH,
    AUX_SAFETY_Temperature_LOW,
    AUX_SAFETY_Temperature_HIGH,
};   															//用于表明是辅助通道安全值的上限还是下限

enum CAN_COMPARE_SIGN            //IV支持6种比较符号，CAN只有两个BIT，只能支持4种，这中间有个转换。
{
    //CAN_COMPARE_SIGN_EQUAL,
    CAN_COMPARE_SIGN_GREATER_THAN,
    CAN_COMPARE_SIGN_LESS_THAN,
    CAN_COMPARE_SIGN_GREATER_THAN_OR_EQUAL_TO,
    CAN_COMPARE_SIGN_LESS_THAN_OR_EQUAL_TO,
    CAN_COMPARE_SIGN_USELESS,
    CAN_COMPARE_SIGN_EQUAL,
};
#pragma pack(1)
typedef struct
{
    unsigned short   AuxChnNo;
    unsigned char    AuxChnType:4;	//
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   DataType:5;
    unsigned short    AckStatus:1;
    //unsigned char    ErrCode;
    float            FloatData;
} ST_Request_Data_Ack;                            // 8byte 数据回复


typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;	//
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   CmdStatus:5;
    unsigned short   IfAnyLimit:1;
    unsigned char    RecvCmd;              //接收到的命令
    unsigned short   StepID;
} ST_Confirm_Ack;                            //回复确认


typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;	//
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   Limit:4;   //?????????!!!待验证
    unsigned short   Reached:2;               //1到达
    //unsigned   char   T2F:1;
    float             FloatData;        //limit status change data
} ST_Limit_Status_Report;

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned  short  OverrunStatus:6;
    float             FloatData;
} ST_Report_Unsafe;

typedef struct
{
    unsigned char    AuxCount;
    unsigned char    AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   reserved1:6;
    unsigned char    reserved2[4];
} CAN_DownLoad_SchMap;                            // 8byte

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned  short  SafetyType:4;             //安全类型 上限/下限等
    float            FloatData;
} CAN_DownLoad_Safety;                            // 8byte

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned short    ParaType:4;                 //?? ??...
    float             FloatData;
} CAN_Download_Sdl;                            // 8byte SDL下载


typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned  short    All:1;               //MAP单个(0)或所有(1)
    unsigned  short    Sign:3;              //比较符  >  <  = ...
    unsigned  short    Dt:1;                   // 时间一阶导数
    unsigned  short    D2t:1;              // 时间二阶导数
    float            FloatData;
} CAN_Download_Limit;                     // 8byte Limits下载

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned short    Rsvd:6;
    unsigned  char    All;               //MAP单个(0)或所有(1)
    unsigned  char    NewClr;            //为下一步准备clear
    unsigned  short   StepID;
} CAN_Clear_Limit;                       // 8byte  清除Limit

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned short    Rsvd:6;
    unsigned  char    All;                 //MAP单个(0)或所有(1)
    unsigned  char    Rsvd1[3];             //预留
} CAN_Clear_Safety;                            // 8byte 清除安全设置值

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned short    Rsvd:6;
    unsigned  char    All;               //MAP单个(0)或所有(1)
    unsigned  char    data_type:4;        //数据类型，原始数据0， 一阶时间导数 1， 二阶时间导数2
    unsigned  char    single_or_repeat:4;  //单个或连续
    unsigned  char    TimeDelta:4;          //预留    可以设置多个等级10ms 100ms 1000ms等（可以不用浪费这么多bit）
    unsigned  char    MetaDelta:4;        //预留，变量delta 大致如上
    unsigned  char    Rsvd1[1];
} CAN_Request_Data;                     // 8byte

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned  short    All:6;                 //MAP单个(0)或所有(1)
    float             m_Data;             //预留
} CAN_Clear_RequsetData;
typedef struct
{
    //unsigned char    AuxChnNo;
    unsigned char     bLogDxdt:1;     //160429
    unsigned char     reserved:7;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
	//unsigned char	IV_GlobalChnNo;
	MsTime            m_Mstime;
} CAN_Trigger_Log;                       // 8byte 触发 check上报数据

typedef struct
{
    unsigned char     AuxChn;
    unsigned char     AuxChnType:4;
    unsigned char     IV_ChnNo:4;
    unsigned short    SN:10;
    unsigned short    All:6;            //单个(0)或所有(1)
    unsigned  char    Enable;        //Enable or Disable
    unsigned  short   StepID;
} CAN_Enable_Sdl;

// 	typedef struct
// 	{
// 		unsigned char    uc_Resverd;   //unused
// 		unsigned char    AuxChnType:4;
// 		unsigned char    IV_ChnNo:4;
// 		unsigned short   SN:10;
// 		unsigned short   Rsvd:6;
// 		unsigned int     m_32bitDO;
// 	}CAN_Set_DOCtrl;

typedef union
{
    unsigned long int  m_32bitDO;
    float              f_SP;
    unsigned short		 m_uMTCSafetyTimeValue;	//MTC温度控制值
} Aux_Ctrl_Value;
typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   Rsvd:6;
    Aux_Ctrl_Value   Set_Value;
} CAN_Set_Ctrl;

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   RunON:1;
    unsigned short   Rsvd:5;
} CAN_Ctrl_RunOn;

typedef struct
{
    unsigned long    Set_value:15;  //15个Bit 可以表示从-16384到16384。 除以100，可以表示从-163.84到163.84的设定值。分辨率为0.01。
    unsigned long  kP:7;   //表示KP的值，从0~127，分辨率为1。
    unsigned long  kI:10;  //表示KI的值，从0~1023， 除以100，可以表示Ki 从0~10.23，分辨率为0.01 	                      //kD=0,tolerance 暂不要了。
} Aux_AIO_PID_Value;
typedef struct
{
    unsigned char    Aux_AI_ChnNo;      //这里放AI的通道号，也就是CtrlValueType3 里的通道号，通道号FF 表示为open control, 因为目前我们不准备支持到128个通道，所以FF可以用作开路控制的标志
    unsigned char    Aux_AI_ChnType:4;  //这里放AI 的类型，也就是CtrlValueType3里的辅助类型
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned short   On_Off_Flag:1;    //DO开关控制， 0：Off,  1:Turn on
    unsigned short   AUX_AO_ChnNO:5;  //用于放DO或者AO的通道号，可放到32个通道
    Aux_AIO_PID_Value   m_PID_Value;
} CAN_SetAIO_Ctrl;
typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char     AuxChnType:4;
    unsigned  char    m_CaliStatus:4;        //0 failed 1 ok
    MsTime            m_MsTime;

} CAN_Time_Sync;                             //用于CAN时间同步

typedef struct
{
    unsigned char    AuxChn;
    unsigned char     AuxChnType:4;
    unsigned  char    Rsvd:4;
    MsTime            m_MsTime;
} CAN_Request_Time;

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned long long startIVIdx:8;
    unsigned long long AuxInfo:48;
} St_AuxStatus;

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned char    AuxChnType:4;
    unsigned char    IV_ChnNo:4;
    unsigned short   SN:10;
    unsigned  short  MV_Type:4;             //MetaVariable 变量类型： 0：ChargeEnergy, 1:Discharge Energy, 2:ChargeCapacity, 3:DisChargeCapacity
    float            FloatData;
} ST_Report_MV;

typedef struct
{
    unsigned char      AuxChnNo;
    unsigned long long startIVIdx:8;
    unsigned long long AuxINFO1:6;
    unsigned long long AuxINFO2:6;
    unsigned long long AuxINFO3:6;
    unsigned long long AuxINFO4:6;
    unsigned long long AuxINFO5:6;
    unsigned long long AuxINFO6:6;
    unsigned long long AuxINFO7:6;
    unsigned long long AuxINFO8:6;
} St_AuxStatusbit;
typedef union
{
    St_AuxStatus AuxInfoAll;
    St_AuxStatusbit AuxInfobit;
} unAuxStatus;


typedef struct
{
    unsigned char    AuxChnNo;
    unsigned short    IV_ChnNo:4;
    unsigned short    Used:1;
    unsigned short    LimitCount:11;
    unsigned char     IfAnyLimitCount;
    unsigned char     SafetyCount;
    unsigned  short    StepID;
    unsigned char     IV_AskFlag;
} CAN_Status_Rpt;

typedef struct
{
    unsigned char    AuxChnNo;
    unsigned short   IV_ChnNo:4; //1 byte
    unsigned short   LimitCount:12;
    unsigned short  StepID;
    unsigned char   IfAnyLimitCount;
    unsigned char   SafetyCount;
    unsigned char   IV_AskFlag;
} IV_AskStatus;

#pragma pack()
typedef struct
{
    unsigned short     m_SN;
    unsigned short     m_StepID;
    unsigned char      m_AuxUnitID;
    unsigned char      m_Checked;
    unsigned char      m_IfAnyLimit;
    unsigned long long m_AuxUintBit;
} St_SNInfo;
typedef struct
{
    unsigned short  m_PosInResTab;
    unsigned short  m_SN;
    unsigned short  m_StepID;
    // unsigned char Used;
} St_AnyLimitSNCount; //用于标记ifany(同SN)处理  返回ifany的SNCOUNT
typedef struct
{
    St_AnyLimitSNCount m_SNInfo[MAXIFANYLIMIT];
    unsigned char      m_Cnt;//自身anylimit个数
} St_IfAnyLimitSNInfo;

typedef struct
{
    St_SNInfo      m_SNInfo[SN_LENGTH];
    unsigned char  m_SNCount;
    unsigned char  m_CheckedCount;
    St_IfAnyLimitSNInfo  m_stAnyLimit;
} St_SNTable;

//#endif



#pragma pack(1)
//////WatchDog

typedef union
{
    bool IsSubMasterSetOpen;
    bool IsSubMasterOpen;
} SubMasterStatus;

typedef struct
{
    unsigned	char	AckType;              //应答类型
    unsigned	char	SN;
    SubMasterStatus     SubStatus;//20161028 status
    unsigned	char	Rsvd[5];
} CAN_WATCHDOGACKDATA;
///// Master --- > submaster////
typedef struct
{
    unsigned   char   RecvCmd;              //接收到的命令
    unsigned   short  SN;
    unsigned   short  StepID;
    unsigned   char   errCode;
    unsigned   char   Rsvd[2];

} CAN_Parallel_Ack;
typedef struct
{
    unsigned short    SN;
    unsigned short m_uRange:2;
    unsigned short m_uCtrlType:13; //rest or current only  CT_IVCH_CURRENT or CT_IVCH_REST
    unsigned short bNeedFilter:1;//用于判断从IV是否需要滤波操作 20160819 zyx
    float m_fCtrlValue;//if current, only high range possible
} CAN_Parallel_SubmasterRx_Start;

typedef struct
{
    unsigned short    SN;
} CAN_Parallel_SubmasterRx_Stop;

typedef struct
{
    unsigned short    SN;
    unsigned short m_uRange:2;
    unsigned short m_uCtrlType:13; //rest or current only  CT_IVCH_CURRENT or CT_IVCH_REST
    unsigned short bNeedFilter:1;//用于判断从IV是否需要滤波操作 20160819 zyx
    float m_fCtrlValue;//if current, only high range possible
} CAN_Parallel_SubmasterRx_StepChange;

typedef struct
{
    float m_fSeries2BoosterVoltage;//unsigned short    SN;  //03.21.2018 CAN传输电压
    float m_fCtrlValue;
} CAN_Parallel_SubmasterRx_UpdateOutput;

typedef struct
{
    unsigned short    SN;
} CAN_Parallel_SubmasterRx_RequestData;

typedef struct
{
	unsigned short    SN;
    unsigned char bNeedFilter:1;//用于判断从IV是否需要滤波操作 20160819 zyx
	unsigned char bRapidlyCommunicate:1;//bCCCV_Ramp:1; // 用于CCCV中开始时Ramp   
	unsigned char bNone:6;
} CAN_Parallel_SubmasterRx_SetFilter;

///// Submaster --> Master ////

typedef struct
{
    //不需要cmd了，已在CAN ID里，这里直接放2个float
    //unsigned short m_uSubUnitID; //maybe not need?
    float m_SubUnitCurrent;
    float m_SubUnitVoltage;
} CAN_Parallel_MasterRx_SubmasterData;

typedef struct
{
    unsigned  char    Rsvd;
    float             m_fData;
} CAN_Parallel_MasterRx_SubmasterCurrentAbn;

typedef struct
{
    unsigned  char    Rsvd;
} CAN_Parallel_MasterRx_SubmasterVoltageAbn;

typedef struct
{
    unsigned  char    Rsvd;
    unsigned short    m_uChanIdx;
    unsigned short    m_uType;
} CAN_Parallel_MasterRx_SubmasterIrregulation;

typedef struct
{
    unsigned  char    Rsvd;
    unsigned short    m_uChanIdx;
    unsigned short    m_uType;
} CAN_Parallel_MasterRx_SubmasterCtrlError;


typedef struct
{
    unsigned short    IV_ChnNo:4;  //chan idx in subunit
    unsigned short    SN:12;
    unsigned short    m_uUnsafeType;  //curr /voltage sys unsafe
    float             m_fData;  //unsafe value
} CAN_Parallel_MasterRx_SubmasterUnsafe;

typedef struct
{
    float    m_ClampVolHigh;
    float    m_ClampVolLow;
} CAN_Parallel_SubmasterRxClampVol;

typedef struct
{
    float    m_ClampVolHigh;
    float    m_ClampVolLow;
} CAN_Parallel_SubmasterRxSimulationClampVol;  //10.04.2017 add by zc

typedef struct
{
    float             m_Vpp;
    float             m_Vmm;
} CAN_IV_SendToM0;

typedef struct
{
    unsigned char    m_Units;
    unsigned char    m_Reserve[7];
} CAN_PowerAdj_Config;
#pragma pack()


///////////////////////BMS //////////////////////////////
//#if(SUPPORT_CAN_BMS == 1)

enum EN_DATA_ENDIAN
{
    e_LittleOrder = 0,
    e_BigOrder = 1,
};
typedef enum
{
    MULTI_PACK_EMPTY = 0x00,   //
    FIRST_PACK_FILL = 0x02,     //
    FIRST_PACK_EMPTY = ~FIRST_PACK_FILL,   //
    SECOND_PACK_FILL  =4,
    SECOND_PACK_EMPTY =~SECOND_PACK_FILL,
    MULTI_PACK_FILL = 0x06,
} EN_MULTI_PACK_STATUS;

#pragma pack(1)
//for CAN Content Union (Data[8])
typedef struct
{
    unsigned char  SN;
    UN_CANID       CANID;
    unsigned char  Bytes[3];
} St_FirstData;
typedef struct
{
    unsigned char  SN;
    unsigned char  Bytes[5];
    unsigned char  FrameType:1;
    unsigned char  bBroadCast:1;
    unsigned char  bOnceTime:1; //one time can msg(CAN MSG period == 0)
    unsigned char  bBigEndian:1; //one time can msg(CAN MSG period == 0)
    unsigned char  Resv:4;
    unsigned char  DLC;
} St_SecondData;
//used in Raw buffers
typedef struct
{
    St_FirstData   m_DataA;
    MsTime         m_Overtime;
} ST_First_Pack;

typedef struct
{
    St_SecondData  m_DataB;
    MsTime         m_Overtime;
} ST_Second_Pack;

//	typedef struct
//  {
//	  unsigned char  SN;
//	  unsigned short m_Period;
//	  unsigned int   m_CANID:29;
//     unsigned int   m_SignalSotp:1;//signal stop 1
//	  unsigned int   m_AllStop:1; //  all Stop 1
//     unsigned int   m_bBroadcast:1;
//	  unsigned char  m_Endian;
//  }ST_ThrirdPack;

typedef struct
{
    unsigned int  SN:8;
    unsigned int  m_Period:23;
    unsigned int  m_Endian:1;
    //next WORD
    unsigned int  m_CANID:29;
    unsigned int  m_SignalSotp:1;//signal stop 1
    unsigned int  m_AllStop:1; //  all Stop 1
    unsigned int  m_bBroadcast:1;

} ST_ThrirdPack;

typedef struct
{
    float m_Data1;
    float m_Data2;
} ST_BMSBroadCast;

typedef struct
{
    unsigned char  m_SN;
    unsigned char  m_IsFDCAN;
    unsigned char  m_baudRate;
    unsigned char  m_bForward;
    unsigned char  m_BitRate;
} ST_SetBMSBaud;
typedef struct
{
    unsigned char m_RxCmd;
    unsigned char m_SN;
    unsigned char m_bStopAll;
    unsigned char m_bStopSignal;
    unsigned int  m_CANID;
} St_CANBMSAck;
typedef struct
{
    unsigned char  Status;	//valid handled overtime?
    ST_First_Pack  m_FirstPack;
    ST_Second_Pack m_SecondPack;
} ST_Multi_Pack;
typedef struct
{
    unsigned char  Status;	//valid handled overtime?
    ST_First_Pack  m_FirstPack;
    ST_Second_Pack m_SecondPack;
    ST_ThrirdPack  m_ThirdPack;
} ST_AuxMulti_Pack;
typedef struct
{
    UWord32 Command_identifier:8;
    UWord32 ErrorCodeLow24Bit:24;
    UWord32 ErrorCodeHigh8Bit:8;
    UWord32 Reserve24Bit:24;

} CAN_Report_Dsp_Error;
typedef struct
{
    Word16    m_wSecondaryVoltage;
    Word16    m_wChannelVoltage;
    Word16    m_wDCVoltageOffset;
    unsigned char    m_btDspBoardType;
    unsigned char    m_btNone;
} ST_DSP_Basic_First_Pack;
typedef struct
{
	unsigned char     m_btSw_Freq;                       
    unsigned char     m_btGrid_Voltage:4;	
	unsigned char     m_btHZ_XMFR:1; 
	unsigned char     m_btReserve:3; 
	
	unsigned char     m_btDSPBoardType;
	unsigned char    m_btNone[1];
	
	Word16            m_wDSP_Version:14;//YYXX 1901,19年第1版
	Word16            m_wDSP_TYorZY:2;//00:TY,01:ZY	   
    unsigned char    CRCL;
    unsigned char    CRCH;
} ST_DSP_Basic_Second_Pack;
typedef struct
{
    Word16               m_wV_KP;
    Word16               m_wV_KI;
    Word16               m_wI_KP;
    Word16               m_wI_KI;
} ST_DSP_Adjust_First_Pack;
typedef struct
{
    unsigned char    m_btNone[6];
    unsigned char    CRCL;
    unsigned char    CRCH;
} ST_DSP_Adjust_Second_Pack;

typedef struct
{
    unsigned char   m_DSP_Indentity;
    unsigned char   m_DSP_SlaveCountOrMasterID;
    unsigned char   m_DSP_SlaveID[6];
    
} ST_DSP_Indentity_First_Pack;
typedef struct
{
	  unsigned char    m_DSP_SlaveID[2];
	  unsigned char    m_btNone[4];
    unsigned char    CRCL;
    unsigned char    CRCH;
} ST_DSP_Indentity_Second_Pack;

typedef struct
{
    unsigned char 	sys_unitID;
    unsigned short  sys_RUNStatus;
    unsigned short  sys_ALARMStatus;
    unsigned char   m_btNone[3];
} CAN_SYSALARMMSG;

#pragma pack()
//#endif
typedef union
{
    unsigned char ucCANData[64];
    float 		fCANData[16];
} CANDATA;

typedef struct
{
    unsigned char  m_SN;
    unsigned char  FrameType:1;
    unsigned char  bBroadCast:1;
    unsigned char  bOnceTime:1; //one time can msg(CAN MSG period == 0)
    unsigned char  bBigEndian:1; //one time can msg(CAN MSG period == 0)
    unsigned char  Resv:4;
    unsigned char  DLC;
    unsigned char  m_reserved;
    unsigned int   CANid;
    CANDATA        CANdata;
} ST_CANMsg;
//////////////////////////////BMS EBD/////////////////////
typedef union
{
    unsigned  char             Data[72];
    float                      fData[16];
    unsigned long long         LLData;
    //////Aux RecV  IV Send
//#if(SUPPORT_AUX==1)
    CAN_DownLoad_SchMap        m_DownSchMap;                   //根据Schedule更新Map
    CAN_DownLoad_Safety        m_AddSafety;                    //添加安全设置
    CAN_Download_Sdl           m_DownSdl;                      //下载SDL参数
    CAN_Download_Limit         m_DownLimit;                    //下载Limit
    CAN_Clear_Limit            m_ClearLimit;                   //清除Limit
    CAN_Clear_Safety           m_ClearSafety;                  //清除safety
    CAN_Request_Data           m_ReqstData;                    //请求数据
    CAN_Clear_RequsetData      m_Clear_RequestData;            //清除请求数据
    CAN_Trigger_Log            m_TrigLog;                      // 触发 check上报数据
    CAN_Enable_Sdl             m_EnableSdl;                    //使能SDL模式
    IV_AskStatus               m_AskStatus;
    CAN_Time_Sync              m_SyncTime;
    //CAN_Set_DOCtrl             m_SetDOCtrl;
    CAN_Set_Ctrl               m_SetCtrl;
    CAN_Ctrl_RunOn             m_CtrlRunOn;										//用于控制温控箱开关 zyx
    CAN_SetAIO_Ctrl            m_SetAIOCtrl;
    //////Aux Send  IV Recv
    ST_Request_Data_Ack        m_DataAck;
    ST_Confirm_Ack             m_ConfirmAck;
    ST_Limit_Status_Report     m_LimitRpt;
    ST_Report_Unsafe           m_Unsafe;
    CAN_Request_Time           m_RequestTime;
    St_AuxStatus               m_AuxStatus;
    ST_Report_MV               m_MetaVariable;
//#endif
    //CAN BMS
    //#if(SUPPORT_CAN_BMS == 1)
// 	St_FirstData               m_BMSFirstData;
// 	St_SecondData              m_BMSSecondData;
    ST_ThrirdPack              m_BMSThirdData;
    ST_BMSBroadCast            m_BMSBrdCast;
    ST_SetBMSBaud              m_BMSBaudMsg;
    St_CANBMSAck               m_CANBMS_Ack;
    ST_CANMsg                  m_BMSData; //181229 zyx
    //#endif

    //Receiving:
    ///// Master --- > submaster////
    CAN_Parallel_SubmasterRx_Start				m_ParallelStart;
    CAN_Parallel_SubmasterRx_Stop					m_ParallelStop;
    CAN_Parallel_SubmasterRx_StepChange			m_ParallelStepChange;
    CAN_Parallel_SubmasterRx_UpdateOutput		m_ParallelUpdateOutput;
    CAN_Parallel_SubmasterRx_RequestData		m_ParallelRequestData;
    CAN_Parallel_SubmasterRxClampVol				m_ParallelClampVolSet;
    CAN_Parallel_SubmasterRxSimulationClampVol					m_ParallelCurrentSimulationClampVolSet;   //09.25.2017 added by zc  //10.04.2017 modified by zc
    CAN_Parallel_SubmasterRxSimulationClampVol					m_ParallelPowerSimulationClampVolSet;   //10.04.2017 added by zc
    CAN_Parallel_SubmasterRx_SetFilter          m_ParallelSetFilter;
    ///// Submaster --> Master ////
    CAN_Parallel_MasterRx_SubmasterData     	 m_Parallel_SubData;
    CAN_Parallel_MasterRx_SubmasterCurrentAbn  m_Parallel_SubCurrAbn;
    CAN_Parallel_MasterRx_SubmasterVoltageAbn	 m_Parallel_SubVoltAbn;
    CAN_Parallel_MasterRx_SubmasterUnsafe	    m_Parallel_SubUnsafe;

    //Sending:
    ///// Master --- > submaster////
    //CAN_Parallel_MasterSend_Start					m_Parallel_MsRequest_Start;
    //CAN_Parallel_MasterSend_Stop			   	m_Parallel_MsRequest_Stop;
    //CAN_Parallel_MasterSend_StepChange			m_Parallel_MsRequest_StepChange;
    //CAN_Parallel_MasterSend_UpdateOutput		   m_Parallel_MsRequest_UpdateOutput;
    //CAN_Parallel_MasterSend_RequestData		   m_Parallel_MsRequest_Data;

    CAN_Parallel_SubmasterRx_Start 					m_Parallel_MsRequest_Start;
    CAN_Parallel_SubmasterRx_Stop						m_Parallel_MsRequest_Stop;
    CAN_Parallel_SubmasterRx_StepChange				m_Parallel_MsRequest_StepChange;
    CAN_Parallel_SubmasterRx_UpdateOutput			m_Parallel_MsRequest_UpdateOutput;
    CAN_Parallel_SubmasterRx_RequestData			m_Parallel_MsRequest_Data;

    ///// Submaster --> Master ////
    CAN_Parallel_MasterRx_SubmasterData				m_Parallel_SubReport_Data;
    CAN_Parallel_MasterRx_SubmasterUnsafe			m_Parallel_SubReport_Unsafe;
    CAN_Parallel_MasterRx_SubmasterCurrentAbn		m_Parallel_SubReport_CurrAbn;
    CAN_Parallel_MasterRx_SubmasterVoltageAbn		m_Parallel_SubReport_VoltAbn;
    CAN_Parallel_MasterRx_SubmasterIrregulation	m_Parallel_SubReport_Irregulation;
    CAN_Parallel_MasterRx_SubmasterCtrlError		m_Parallel_SubReport_CtrlError;
    CAN_Parallel_Ack                             m_Parallel_Ack;

    //watchdog ack
    CAN_WATCHDOGACKDATA								m_WatchdogACK;
    CAN_IV_SendToM0									m_IV_SendToM0;
    CAN_PowerAdj_Config                             m_IV_SendConfig;
    CAN_Report_Dsp_Error                   m_Dsp_Report_Error;
    ST_DSP_Basic_First_Pack                m_Dsp_Basic_First_Pack;
    ST_DSP_Basic_Second_Pack               m_Dsp_Basic_Second_Pack;
    ST_DSP_Adjust_First_Pack               m_Dsp_Adjust_First_Pack;
    ST_DSP_Adjust_Second_Pack              m_Dsp_Adjust_Second_Pack;
    ST_DSP_Indentity_First_Pack            m_Dsp_Indentity_First_Pack;
    ST_DSP_Indentity_Second_Pack           m_Dsp_Indentity_Second_Pack;

    //SystemAlarm
    CAN_SYSALARMMSG                        m_SysAlarmMsg;

} Un_Data; //8byte

typedef struct
{
    unsigned char   FrameType;
    unsigned char   DLC;
    unsigned char   bBMSCmd;
    unsigned char   SubBmsCmd;
    unsigned char   bBroadCast;
    unsigned char   bBMSOnceTimeMsg;//send can msg (once time )
    unsigned char   m_ChnNo;  //for bms transfer(canid to ivchnNo)
    UN_CANID        Un_CANID;
    Un_Data         Un_Content;
} MP_CAN_MSG;                   //CAN发送单个报文结构

typedef struct
{
    unsigned short   ByteLength;// 该结构实际长度：2+2+SZIEOF(UN_CANID)+SIZEOF(Un_Data)
    unsigned char   res[2];
    UN_CANID        Un_CANID;
    Un_Data         Un_Content;
} MP_MCU_MSG;                   //RUDP发送单个报文结构
//#if(SUPPORT_CAN_BMS==1)
typedef struct
{
    unsigned char m_chn;
    MP_CAN_MSG    Msg;
} MP_CANBMS_MSG;                   //CAN发送单个报文结构
//#endif

//end of file
#endif
