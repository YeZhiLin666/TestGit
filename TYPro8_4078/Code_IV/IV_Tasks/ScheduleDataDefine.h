/*****************************************************
File Name: ScheduleDataDefine.h
Version:        V1.0
Date:                2012/11/12
Description:
大数据体各种相关结构，命令及宏定义
Others:
History：

******************************************************/
#pragma once

#include "../Entry/includes.h"

#include "../../COMMON_OS/LPC4078CORE/TIMERS.h"
#include "../../COMMON_OS/LPC4078CORE/RTC.h"
#include "../../COMMON_OS/protocols/SF_Hwf.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue.h"

#include "../../common_OS/protocols/SF_Hwf.h"
#include "../IV_Tasks/SimuBuffer.h"

#define IV_BUFFER1_BETA                      20
#define IV_BUFFER1_ALPHA                     20

#define FRAM_BASE                            0x80        // Fram存储Config的起始地址...  //IVconfig大概占用6K左右的空间， PID从16K处地址开始。
#define FRAM_NONE_BASE                       0X4010      // 还留可用2.3K字节  (预留下的空间）["2.3K字节"]
#define FRAM_CANBMS_MAP_BASE                 0X4940      // CANBMS_MAP占用68字节 
#define FRAM_PARALLEL_BASE                   0X4990      // PARALLEL占用636字节  
#define FRAM_IV_BOARDTYPE                    0X4D00      //  
#define FRAM_PID_BASE                        0X4D64      // PID占用6.1K字节 // sizeof(St_IV_ChannelPID)*16=384*16=6144  
#define FRAM_AUXMAP_BASE                     0X6570      // AUX MAP占用4.1K字节 40+128*8*4=4136  (请保持AUXMAP表在最后，以方便日后对大量辅助的支持）

#define FRAM_REMAIN_LENGTH                   (0X8000 - FRAM_AUXMAP_BASE -  FRAM_AUXMAP_LENGTH)   // 256K/8=32K    剩余大小空间1.1K


//扩大到5.7K存MAP表,512*11=5632   相当于一个IVMCU最大可支持4个AUXMCU到辅助512个通道， 平均512/16=32 ,每个IV可带32个Aux通道。
#define MAXAUXMCUCHANNE       (512) // (4*8*16)    //IVMCU最大可支持4个AUXMCU到辅助512个通道
#define FRAM_AUXMAP_LENGTH    (0X1628) // 5672    //IVMCU最大可支持4个AUXMCU到辅助512个通道,每通道11个字节  // 40+512*11=5672
#define MAX_ADC_CODE_QUE             32
// MAP表计算出实际一维字节数量   以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。

#define   Ec0_Ttype	0.0f
#define   Ec1_Ttype	3.8748106364e1f
#define   Ec2_Ttype	3.329222788e-2f
#define   Ec3_Ttype	2.0618243404e-4f
#define   Ec4_Ttype	-2.1882256846e-6f
#define   Ec5_Ttype	1.0996880928e-8f
#define   Ec6_Ttype	-3.0815758772e-11f
#define   Ec7_Ttype	4.547913529e-14f
#define   Ec8_Ttype	-2.7512901673e-17f
const float   Ec0_Ktype	= 1.7600413686e1f;
const float   Ec1_Ktype	= 3.8921204975e1f;
const float   Ec2_Ktype	= 1.8558770032e-2f;
const float   Ec3_Ktype	= -9.9457592874e-5f;
const float   Ec4_Ktype	= 3.1840945719e-7f;
const float   Ec5_Ktype	= -5.6072844889e-10f;
const float   Ec6_Ktype	= 5.6075059059e-13f;
const float   Ec7_Ktype	= -3.2020720003e-16f;
const float   Ec8_Ktype	= 9.7151147152e-20f;
const float   Ec9_Ktype	= -1.2104721275e-23f;
const float   Ea0_Ktype	= 1.185976e2f;
const float   Ea1_Ktype	= -1.183432e-4f;

const float   Ec0_Jtype	= 0.0f;
const float   Ec1_Jtype	= 5.038118715e1f;
const float   Ec2_Jtype	= 3.0475836930e-2f;
const float   Ec3_Jtype	= -8.5681065720e-5f;
const float   Ec4_Jtype	= 1.3228195295e-7f;
const float   Ec5_Jtype	= -1.705295837e-10f;
const float   Ec6_Jtype	= 2.0948090697e-13f;
const float   Ec7_Jtype	= -1.2538395336e-16f;
const float   Ec8_Jtype	= 1.5631725697e-20f;

const float   Ec0_Etype	= 0.0f;
const float   Ec1_Etype	= 5.86655087e1f;
const float   Ec2_Etype	= 4.5032275582e-2f;
const float   Ec3_Etype	= 2.8908407212e-5f;
const float   Ec4_Etype	= -3.3056896652e-7f;
const float   Ec5_Etype	= 6.502440327e-10f;
const float   Ec6_Etype	= -1.9197495504e-13f;
const float   Ec7_Etype	= -1.2536600497e-15f;
const float   Ec8_Etype	= 2.1489217569e-18f;
const float   Ec9_Etype	= -1.4388041782e-21f;
const float   Ea0_Etype	= 3.5960899481e-25f;

enum IR_SOURCE
{
	SET_NONE = 0,
    SET_SCH = 1,
    SET_CCCV_WRM = 2,
    SET_STEP = 3,
    SET_CCCV = 4,
    SET_CCCV_TB = 5,
    SET_CC_Rising = 6,
    SET_CCCV_RAMP = 7,
	SET_ESTIMATE = 8,
    SET_UNKNOW = 9,
};

enum FRAM_ADDRESS
{
    CHANNEL_CFG,
    NONE_EMPTY,
    CANBMS_MAP,
    PARALLEL_BATCH,
    BOARDTYPE_CFG,
    PID_CFG,
    REMAIN_EMPTY,
    MAX_ADDRESS
};

//#define MAXCHANNELNO                         16        //最大16通道
//#define MAXMSGNO                             16        //信息缓冲区最多16条

//#if SUPPORT_PARALLEL == 1
//#define MAXCHANNELNO                         1        //最大16通道
#define MAXUNITNO			40		//最多40 unit/system
//#endif
#define IV_SAMP_CURRENT_FILTER_DEPTH         256       //定义IV通道采样数据电流滤波深度最大值，具体的滤波深度将变为上位机动态配置
#define IV_SAMP_VOLTAGE_FILTER_DEPTH         256       //定义IV通道采样数据电压滤波深度
#define IV_FRAM_MESSAGE_DEPTH                20        //定义FRAM TASK的消息队列深度
#define IV_SCHEDULE_MESSAGE_DEPTH            4         //定义SCHEDULE TASK的消息队列深度
#define IV_STEP_MESSAGE_DEPTH                4         //定义STEP TASK的消息队列深度
#define IV_PARSE_MESSAGE_DEPTH               4         //定义PARSE TASK的消息队列深度

//safetycheck
#define  DEF_ACCURACY                     0.0005f      //默认精度，默认为 万分之5
#define  DEF_CTRL_VALUE_ERROR_FACTOR      100          //默认超出精度100倍，被认为是控制值异常发生，默认为5%
//#define  CTRL_VALUE_CHECK_PERIOD          1 // 60           //控制值检查周期，时间单位为秒 //1805222 zyx
#define  IRREGULAR_CHECK_PERIOD           1 // 60           //合理性检查周期，时间单位为秒
#define  CTRL_VALUE_CHECK_PERIOD           1 // 10           //控制值检查周期 时间单位为秒 //180808 zyx
// #define  IRREGULAR_VOLT_CHECK_COUNT       5 // 10           //不合理电压检查触发错误次数
// #define  IRREGULAR_CHECK_MAX_STEPTIME     4*3600       //不合理步时间检查的时间最大值: 4小时
// #define  IRREGULAR_CHECK_ERRORCAPA        1.2f         //不合理容量检查的错误触发系数
// #define  IRREGULAR_CHECK_WARNINGCAPA      1.05f        //不合理容量检查的报警触发系数
// #define  IRREGULAR_CHECK_ERRORVOLT        0.2f         //不合理电压检查的错误触发系数

enum eGroupState
{
    eGroupState_Normal,
    eGroupState_Master,
    eGroupState_Submaster,
    eGroupState_Slave,
    eGroupState_Serial,
    eGroupState_Booster,
};
typedef struct
{
    unsigned char CanID;
    unsigned char ChanIdx;
} St_ChanAddr;
typedef struct
{
    unsigned char State;                            //通道种类
    St_ChanAddr   Master;                           //主通道寻址
    unsigned char SlaveCount;                       //从通道个数
    unsigned char SubmastCount;                     //从Unit个数
    St_ChanAddr   Submaster[MAX_UNIT_PER_GROUP];    //所有从Unit寻址
    //以上为配置级别，以下为运行级别
    bool          bMultiHigh;                       //是否正在运行并联HIGH量程，多通道打开状态
} St_Grp;

//通道三态锁
enum
{
    CHN_LOCK_IDLE = 0x01,
    CHN_LOCK_SCH,
    CHN_LOCK_THIRD_PARTY,
};

enum MESSAGE_SOURCE
{
    Message_Parse = 0,
    Message_Schedule,
    Message_Step,
    Message_MetaVariable,
    Message_3rdPart,
    Message_Fram,
    Message_Sample,
};


//以下Step相关 枚举定义 //
enum
{
    CCS_OFF,
    CCS_REST,
    CCS_I,
    CCS_V,
    CCS_EXTERNAL,
    CCS_OTHER=50,
}; //控制状态

enum
{
    CT_REST,
    CT_VOLTAGE,
    CT_CURRENT,
    CT_OFF,
}; //控制类型

enum //STEP状态
{
    STEPIDLE,
    STEPLOADSTEP,
    STEPTRANSITION,
    STEPRUNNING,
    STEPWAIT,
    STEP_RELAX,
    STEP_AFTER_RELAX,
// 	STEP_WAIT_ParallelSlave_PowerReady,   //等待并联从通道电源稳定
};

enum    //StepLimit状态
{
    STEPLIMITSTART,
    STEPLIMITNOTREACH,
    STEPLIMITCHECK,
    STEPLIMITGOTONEXT,
    STEPLIMITREACH,
    STEPLIMITLOOP,
    STEPLIMITEND,
};

enum    //CV 状态
{
    STEP_CV_Init,
    STEP_CV_RAMP,
    STEP_CV_KEEP,
    STEP_CV_IR,
    STEP_CV_AFTER_IR,
    STEP_CV_TEST,
};

enum    //CCCV 状态
{
    STEP_CCCV_INIT,
    STEP_CCCV_RAMP,
    STEP_CCCV_RELAX,
    STEP_CCCV_AFTER_RELAX,
    //STEP_CCCV_WAIT2CV,
    STEP_CCCV_CC,//STEP_CCCV_START,
    STEP_CCCV_CV,
};
enum //CCCV RAMP
{
    NO_RAMP,
    RAMP_INIT,
    FAST_RAMP,
    SLOW_RAMP,
    RAMP_IR_CALCULATE,//RAMP_IR_TEST,
};

enum    // 06.28.2017 TURBO BOOST 状态 by ZC
{
    STEP_CCCV_TB_START,
    STEP_CCCV_TB_CC,
    STEP_CCCV_TB_CV,
    STEP_CCCV_TB_CC_DISCHARGE,
};

enum	//LogLimit状态
{
    STEP_CC_TB_START,       //起始步
    STEP_CC_TB_CHARGE,			//充电步
    STEP_CC_TB_DISCHARGE,		//放电步
};
enum	//LogLimit状态
{
    LOGLIMITSTART,
    LOGLIMITIFRUNNING,
    LOGLIMITCHECK,
    LOGLIMITSEND,
    LOGLIMITEND,
};

/*Schedule 相关枚举定义*/

enum
{
    PCASSIGNOK,
    PCASSIGNFAIL,

    PCSTARTOK,
    PCSTARTFAIL,

    PCSTOPOK,
    PCSTOPFAIL,
};     //大数据体中添加 用于确认属于那种行为的回复

enum
{
    NEXTSTEP,
    ENDTEST,
    UNSAFE,
    SETVARIABLE=0x0011,
    LOGLIMIT,
};	//Limit _Arrive到达时下一步响应处理



////safety 定义/////////////

enum
{
    CtrlValueCheck_Null,
    CtrlValueCheck_Current,
    CtrlValueCheck_Voltage,
    CtrlValueCheck_Power,
    //CtrlValueCheck_Load,
};
enum
{
    SAFETY_CHECK_SYS = 0x22,
    SAFETY_CHECK_SCH,
};

#define SAFETY_CHECK_PERIOD     1000 // 100ms                 //超限时间    这里将来可以根据硬件来使用不同的时间
#define SAFETY_CHECK_RESUME     10000 //1000ms  恢复sch检测时间

//ErrorFlag    每位代表一个含义
/*enum
{
SAFETY_CHECK_ERROR_I = 0,
SAFETY_CHECK_ERROR_V,
SAFETY_CHECK_ERROR_P,
SAFETY_CHECK_ERROR_SCH,
SAFETY_CHECK_ERROR_SYS,
SAFETY_CHECK_ERROR_THIRD,
SAFETY_CHECK_ERROR_MAXVALUE,
};*/

enum
{
    SAFETY_V = 0,
    SAFETY_I,
    SAFETY_P,
    SAFETY_SCH,
    SAFETY_SYS,
    SAFETY_THIRD,
    SAFETY_MAXVALUE,
};

////////////////////////////////////////////////////
// following is the definitions that will be used in this section :checkitem.
#define NO_CHECK		0
#define PI_CHECK		1
#define V_CHECK			2
#define Hw_CHECK		4
#define PULSE_CHECK 8

// following defines the STATUS for StatusProcess() Status Machines.
#define SAFE		0
#define SAFE2UNSAFE	1
#define UNSAFE		2
#define UNSAFE2SAFE	4


//#if SUPPORT_PARALLEL == 1
#ifndef MXUNITNO
#define MAXUNITNO			40		//最多40 unit/system
#endif
//#endif




//////////////////////////

// //Schedule 状态
// enum
// {
// 	NOTREADY,
// 	IDLE,
// 	START,
// 	RUNNING,
// 	GOPAUSE,
// 	RESUME,
// 	GOSTOP,
// 	FIXEDUNSAFE,
// 	DYNAMICUNSAFE,
// 	WAITFORAUX,
// };



//响应列表
enum
{
    //***********************************************
    //				Sch Mgr 信息输入
    //***********************************************
    Sch_Assign_Msg=1,

    Sch_Stop_Msg,
    Sch_Stopped_Msg,

    Sch_Continue_Msg,

    Sch_Run_Msg,
    Sch_Started_Msg,

    Sch_Pause_Msg,
    Sch_JumpA_Msg,
    Sch_JumpM_Msg,
    Sch_Resume_Msg,

    Sch_LimitArrive_Msg,
//	Sch_LimitPropertyChange_Msg,
    Sch_LimitStatusChange_Msg,

    Sch_OnSafe_Msg,
    Sch_OnUnsafe_Msg,
    //Sch_OnUnsafe_Msg,
    //Sch_OnSafe_Msg,

    //***********************************************
    //				Step Mgr 信息输入
    //***********************************************
    Step_Stop_Msg,
    Step_Run_Msg,
    Step_JumpA_Msg,
    Step_JumpM_Msg,

    //***********************************************
    //				Sch Mgr 信息对PC输出
    //***********************************************
    PC_AssignRpt_Msg,
    PC_StopRpt_Msg,
    PC_Started_Msg,
    //PC_PauseRpt_Msg,
    //PC_JumpARpt_Msg,
    //PC_JumpMRpt_Msg,
    //PC_ResumeRpt_Msg,
    PC_LimitArrive_Msg,
    PC_LimitChange_Msg,
    //PC_UnsafeRpt_Msg,
    //PC_LimitStatusChanged_Msg,
    PC_Abnormal_Msg,
    PC_BackToNormal_Msg,
    PC_Unsafe_Msg,
    PC_Error_Msg,
    PC_Jump_Msg,
    PC_GoStop_Msg,
    PC_AcimRelayOn_Msg,
    PC_AcimRelayOff_Msg,

    SIMU_ASSIGN_MSG,   //12.22.2013ADD
    //************************************************
    //                  AUX 对Schedule信息输入
    //************************************************
    AUX_Unsafe_Msg,
    AUX_ACKFAIL_Msg,
    Para_SubUnsafe_Msg,
    CAN_BMS_Unsafe_Msg,
    SMB_Lost_Unsafe_Msg,  //add by zc 12012017
};
//aux 数据类型
// enum
// {
// 	ORIG_DATA,
// 	DX_DT_DATA,
// 	D2X_DT2_DATA,
// 	DATA_TYPE_ERROR,
// };
enum {
    LOG_DELTA_NONE,
    LOG_DELTA_TIME,
    LOG_DELTA_META,
    LOG_DELTA_ALL,
};
enum
{
    LOG_TIME_1MS,
    LOG_TIME_10MS,
    LOG_TIME_20MS,
    LOG_TIME_50MS,
    LOG_TIME_100MS,
    LOG_TIME_1000MS,
    LOG_TIME_2000MS,
    LOG_TIME_5000MS,
};
enum
{
    SAFETYSN,
    LIMITSN,
};


typedef struct
{
    float         m_fEndTime;        //input
    float         m_fValue;          //input

    signed short  m_BufIdx;          //output
    unsigned char m_bEnded;
    unsigned long m_PointIdx;        //output

    bool          m_bValid;          //simulation is working?
    bool          m_bLogLimitValid;  //LogLimit是否起作用？
    bool          m_bCriticalLog;    //160425,add by hepeiqing,for huawei's request,log last point of simulation cycle.
    //使用simulation模拟慢速pulse功能，以下变量因此而增加：
    bool          m_bRepeat;          //这个simulation要做cycle?
    bool          m_bExtraLog;        //这个simulaiton是否需要进行额外LOG?
    bool          m_bLogCycle;       //这个cycle需要进行LOG？
    bool          m_bStageLogged;    //本stage的是否LOG了第一个点？
    bool          m_bStarted;        //Simulation启动了么?
    float         m_fStartTime;      //Simulation启动的起始时间
    unsigned long m_LogInterval;     //每N个cycle进行一次LOG
    unsigned long m_CycleIdx;        //Cycle的序号
    MsTime        m_CycleStartTime;  //simulation cycle的起始时间
    MsTime        m_StageDacTime;    //Stage的第一次DAC起始时间
    float         m_fCycleTime;      //每个循环的周期
} St_SimuRunInfo;

typedef struct
{
    float         m_fStgTime;        //input

    bool          m_bEnded;
    unsigned long m_CycleIdx;        //Cycle的序号
    unsigned long m_StageIdx;        //Stage的序号

    MsTime        m_CycleStartTime;  //simulation cycle的起始时间

    float         m_fGain;           //用于输出值计算
    float         m_fOffset;         //用于输出值计算

    float         m_fBase;           //循环伏安循环做完后的DAC输出值，，客户设定, 一般为0
} St_CvRunInfo;

typedef struct
{
    unsigned char AUX_CAN_ID;	   	//辅助通道CANID号
    unsigned char AUX_Type; 	   	//辅助通道类型
    unsigned short AUX_VirtualID;	//辅助通道逻辑编号
    unsigned short AUX_PhysicalID;	//辅助通道物理编号
    unsigned char Report;           //回复ok or fail
} ST_IVMAP;
typedef struct
{
    unsigned char    m_LimitCheck:1;
    unsigned char    m_ReportValue:1;
    unsigned char    m_Log:1;
    //you can add other here!
    unsigned char    m_Updata:1;
    unsigned char    RVD:4;
} St_AUX_VALUE_BITS;

typedef union
{
    unsigned char      m_Char;    //采样任务，可以在这里赋值0xFF;
    St_AUX_VALUE_BITS  m_Bits;
} St_AUX_VALUE;

typedef struct
{
    float aux_data;			//原始数据
    float aux_dx_dt;		//对时间的一阶导数
    float aux_d2x_dt2;  //对时间的二阶导数
    //bool  updated;			//采样是否更新

    St_AUX_VALUE   updated_aux_data;
    St_AUX_VALUE   updated_aux_dxdt;
    St_AUX_VALUE   updated_aux_d2xdt2;
} ST_AUX_DATA;					//用于存放辅助的数据

//typedef struct
//{
//	unsigned char IV_ChnIdx;
//	unsigned char Status;
//	unsigned short SN;
//
//}ST_LIMIT_CHANGE;
//此结构用于暂存CAN上载数据
typedef struct
{
    unsigned char AUX_CANID;
    unsigned char IV_ChnIdx;
    unsigned char Aux_Type;
    unsigned short Aux_ChnIdx;
    unsigned char Data_type;
    float         Aux_Data;
} ST_AUX_UPLOADATA;
typedef struct
{
    unsigned char m_IV_ChnNo;  	//IV通道号
    unsigned char m_aux_type;		//辅助类型
    unsigned short m_aux_virtual_chn; //辅助虚拟通道号
    unsigned char m_data_type; //数据类型 orign or dxdt ..
    unsigned char m_single_or_repeat; //单次还是重复发送
    unsigned  char    m_TimeDelta;      //时间delta
    unsigned  char    m_MetaDelta;
} ST_REQUEST_AUXDATA;


typedef struct
{
    St_SchSafeData  m_AUXSafety;
} ST_AUXSAFETYCFG;

typedef struct
{
    unsigned char m_UnitId;
    unsigned short m_PhysicalChannelIdx;
} St_AUXchannelInfo;
typedef struct    //来自PC到IV 的信息结构(待确认)
{
    unsigned short m_Aux_UnitID[MAXAUXUNIT];
    unsigned short m_Start_InUnit[MAXAUXUNIT];   //每个Unit的起始通道
    unsigned short m_End_InUnit[MAXAUXUNIT];    //每个Unit的终止通道
    unsigned short m_ActualTotalAuxUnit;					//实际unit 总数
} ST_PC2IV_MAP;


// typedef struct    //来自PC到IV 的信息结构   <<辅助通道的硬件配置和通道分配>> 表4 逻辑大表
// {
// 	unsigned char m_IVCh_Index;     // 在该unit内的IV通道序号

// //	St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAXAUXTYPE][MAX_CH_IN_AUX_UNIT];  // 一个IV通道分到的辅助通道  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
//     St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAXAUXTYPE][MAX_MAP_COUNT_PER_IV_CHAN];  // 一个IV通道分到的辅助通道  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
// 	unsigned short m_Total_In_OneIVCh[MAXAUXTYPE];  // 同种辅助类型下通道总数量  	//--new add---新增加

// } St_PC2IVUnit4AuxMap;  //  From PC to IVUnit for (about) AuxMap
// 对于某个IVMCU， 得用这么一个结构来描述 St_PC2IVUNIT4AuxMap    st_AllAuxMap [MAX_IVCH_PER_IVMCU]


//typedef struct    //来自PC到IV 的信息结构(待确认)
//{
////	unsigned char m_Aux_UnitID[MAXAUXUNIT];
////  	unsigned char m_Start_InUnit[MAXAUXUNIT];   //每个Unit的起始通道
////  	unsigned char m_End_InUnit[MAXAUXUNIT];    //每个Unit的终止通道
////	unsigned char m_PhysicalID[MAXAUXUNIT][MAX_AUXCH_INDEX];  //索引号按照顺序排列最多64个
//	St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
//	unsigned char m_ActualTotalAuxIndex;					//实际Incex总数
//	unsigned char m_AcutalIndexID[MAX_AUXCH_INDEX];         //存储用于检索的索引号(VirtualID)
//} ST_INTRACANNET;
typedef struct
{
    St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
    unsigned short m_ActualTotalAuxIndex;					//实际Incex总数
    unsigned short m_AcutalIndexID[MAX_AUXCH_INDEX]; 		//存储用于检索的索引号(VirtualID)

} ST_CANNET;


typedef struct
{
    St_AUXchannelInfo m_AuxChannelInfo[MAX_AUXCH_INDEX];
    unsigned short m_ActualTotalAuxIndex;					//实际Incex总数
    unsigned short m_AcutalIndexID[MAX_AUXCH_INDEX];         //存储用于检索的索引号(VirtualID)
} ST_IV4AUX;
typedef struct
{
    unsigned char m_Aux_UnitID[MAXAUXUNIT];
    unsigned char m_Aux_UnitCANPort[MAXAUXUNIT];
    unsigned char m_Total_Aux_Unit;
} ST_AUXUNITFG;   //存储辅助类型对应的Unit信息
typedef struct
{
    ST_CANNET  m_IntraCanNet;            //每个通道内网配置
    ST_CANNET  m_OuterCanNet;			  //每个通道外网配置
    ST_IV4AUX 		m_iv4aux;               //IV内部辅助配置
    ST_AUXUNITFG   m_AuxUnitCfg;				//Unit级别的配置信息
    unsigned char   m_MapConfig;                //配置信息，按照位处理
} ST_IV_MAPCONFIG;


// typedef struct //dac infomation
// {
// 	unsigned char                m_bHaveVoltageDac;
// 	unsigned char                m_bHaveClampVdac;
// 	unsigned char                m_bChannelBasedClampVdac;

//   unsigned char                m_bCurrRngCount;
//   unsigned char                m_bVoltRngCount;

// 	unsigned long long           m_uSettingTime;
// }St_DacInfo;

/*typedef struct
{
signed long                  m_FragIdx;
signed long                  m_FragCount;
signed long                  m_ByteCount;
signed long                  m_PointCount;

unsigned char                m_bDownloading;  //正在下载？

St_MD5                       m_MD5;
}St_SimuDownMgr;*/
#pragma pack(1)
typedef struct
{
    unsigned long     Second;
    unsigned long     Us;
} TIME2US;
#pragma pack()
/*以下是通道Metavariable管理相关结构定义*/
typedef union  //Un_ValueConversion
{
    double                       m_double;
    float                        m_float;
    MsTime						 m_Time;

    TIME2US                      m_Time2us;

    unsigned long long int       m_uInt64;
    signed long long int         m_sInt64;
    unsigned long                m_uInt32;
    signed long                  m_sInt32;
    unsigned short               m_uInt16;
    signed short                 m_sInt16;
    unsigned char                m_uInt08;
    signed char                  m_sInt08;

    unsigned char                m_ucData[16];
    unsigned short               m_u16Data[8];
    unsigned long                m_u32Data[4];
    unsigned long long           m_u64Data[2];
    float                        m_fData[4];
} Un_ValueConversion;
typedef struct //st_GetMetaValue
{
    float                        m_fValue;
    //	unsigned char                m_bNeedCalculate;   //这个变量需要经过计算才能使用
    unsigned char                m_bNeedRecal;       //这个变量需要重新计算了
    //float                        m_fReserved;  //无用的填充字节
    Un_ValueConversion           m_Value;
} st_GetMetaValue;

typedef struct
{
    float                        m_fCurrent;
    float                        m_fVoltage;
    unsigned long                m_CurrA;
    unsigned long                m_CurrB;
    unsigned long                m_VoltA;
    unsigned long                m_VoltB;
    unsigned char                m_uCurrCount;
    unsigned char                m_uVoltCount;

    unsigned long                m_uCurrAfterFilter;
    unsigned long                m_uVoltAfterFilter;

    float                        m_fCurrentBeforeRC;
} St_IV_Buffer1;

//#if(SUPPORT_CAN_BMS==1)
typedef struct
{
    float                        m_fValue;
    unsigned char                m_bNeedCalculate;   //这个变量需要经过计算才能使用
    unsigned char                m_bNeedRecal;       //这个变量需要重新计算了
    unsigned char                m_bNeedLog;
    //float                        m_fReserved;  //无用的填充字节
    Un_ValueConversion           m_Value;
} ST_BMS_MetaValue;

//#endif
typedef struct //PV_Info
{
    //   double                       m_dChargeCapacity;
    //   double                       m_dDischargeCapacity;
    //   double                       m_dChargeEnergy;
    //   double                       m_dDischargeEnergy;
    unsigned                     m_CCapacity:1;
    unsigned                     m_DCapacity:1;
    unsigned                     m_CEnergy:1;
    unsigned                     m_DEnergy:1;
} St_CapaEnerUpdated;

typedef struct //DV_Info
{
    MsTime                       m_LastLogTime;
    float                        m_fLastLogVoltage;
    float                        m_fLastLogCurrent;
} St_DV_Info;

typedef struct //St_TimerRegister
{
    double                         m_dResult;
    MsTime                         m_StartTime;
} St_TimerRegister;

typedef struct //St_CapacityRegister
{
    double                         m_dResult;
    double                         m_StartCapacity;
} St_CapacityRegister;

typedef struct //St_TimeBitStatus
{
    unsigned                       TestTime:1;
    unsigned                       StepTime:1;
    unsigned                       Timer1:1;
    unsigned                       Timer2:1;
    unsigned                       Timer3:1;
    unsigned                       Timer4:1;
} St_TimeBitStatus;
typedef union  //Un_TimeStatus
{
    St_TimeBitStatus               m_Bits;
    unsigned char                  m_Char;
} Un_TimeStatus;

typedef struct //St_CapaEnerBitStatus
{
    unsigned                       Capa1:1;
    unsigned                       Capa2:1;
} St_CapaEnerBitStatus;
typedef union  //Un_CapaEnerStatus
{
    St_CapaEnerBitStatus           m_Bits;
    unsigned char                  m_Char;
} Un_CapaEnerStatus;

typedef struct //St_IsampBitStatus
{
    unsigned                       Iset:1;
    unsigned                       NormalizedI:1;
    unsigned                       CalCapacity:1;
    unsigned                       DacOutput:1;
    unsigned                       SimuSamp:1;
} St_IsampBitStatus;
typedef union  //Un_IsampStatus
{
    St_IsampBitStatus              m_Bits;
    unsigned char                  m_Char;
} Un_IsampStatus;

typedef struct //St_VsampBitStatus
{
    unsigned                       Iset:1;
    unsigned                       NormalizedV:1;
    unsigned                       CalCapacity:1;
    unsigned                       DacOutput:1;
    unsigned                       SimuSamp:1;
} St_VsampBitStatus;
typedef union  //Un_VsampStatus
{
    St_VsampBitStatus              m_Bits;
    unsigned char                  m_Char;
} Un_VsampStatus;

typedef struct //TC_Info
{
    //unsigned  long                 m_u32Counter1;
    //unsigned  long                 m_u32Counter2;
    //unsigned  long                 m_u32Counter3;
    //unsigned  long                 m_u32Counter4;

    St_TimerRegister               m_Timer1;
    St_TimerRegister               m_Timer2;
    St_TimerRegister               m_Timer3;
    St_TimerRegister               m_Timer4;

    St_CapacityRegister            m_ChargeCapacity1;
    St_CapacityRegister            m_ChargeCapacity2;
    St_CapacityRegister            m_DischargeCapacity1;
    St_CapacityRegister            m_DischargeCapacity2;
    St_CapacityRegister            m_ChargeEnergy1;
    St_CapacityRegister            m_ChargeEnergy2;
    St_CapacityRegister            m_DischargeEnergy1;
    St_CapacityRegister            m_DischargeEnergy2;

    //Un_TcStatus                    m_Status;      //剥离出去，因为放在这里不好统一调用memset置位.
} St_TC_Info;
typedef struct //St_Metavariable
{
    //St_PV_Info                         m_PV_Info;      //Present Value
    //St_PV_Info                         m_LS_Info;      //Last Step value
    //St_PV_Info                         m_LC_Info;      //Last Cycle value
    //St_PV_Info                         m_LL_Info;      //Last Log value//运行时参数
    St_TC_Info                         m_TC_Info;      //Test Counter value
    St_DV_Info                         m_DV_Info;      //DV time/voltage/current//运行时参数
} St_Metavariable;

/*以下是通道Step管理相关结构定义*/
typedef struct //St_StepRunningInfo
{
    /*unsigned char m_bIfMyLimit;
    unsigned char m_bIfMyLimit_Reached;*/
    unsigned char           bSpecialStepLimit;               //this is a special step?,such as IR
} St_StepRunningInfo;
typedef struct //St_StepInfo
{
    St_StepData             m_Data;          //来自于从schedule数据块直接 内存拷贝 过来的值
    St_StepRunningInfo      m_RunningInfo;   //下位机维护的运行时变量
} St_StepInfo;



/*以下是通道Simulation管理相关结构定义*/
// typedef struct //St_NormalSimuPoint
// {
//     float                   m_fTime;
//     float                   m_fValue;
// }St_NormalSimuPoint;
// typedef struct //St_BatterySimuPoint
// {
//     float                   m_fCapacity;
//     float                   m_fVoltage;
//     float                   m_fDCR;
// }St_BatterySimuPoint;
// typedef union  //Un_SimulationPoint
// {
//     St_NormalSimuPoint   m_Normal[IV_NORMAL_SIMULATION_POINT_NUMBER];
//    // St_BatterySimuPoint  m_Battery[IV_BATTERY_SIMULATION_POINT_NUMBER];
// }Un_SimuPoints; //12.22.2013ADD

//typedef struct //St_SimulationInfo     //12.22.2013ADD
//{
//    unsigned long           m_u32PointIdx;       //当前正在使用的"点"序号
//    signed char             m_scSimuBufIdx;      //指向当前使用的Simulation Buffer的序号指针
//    unsigned char           m_ucReserved[2];
//}St_SimulationInfo;

/*以下是通道Formula管理相关结构定义*/
typedef struct //St_FormulaInfo
{
    St_FormulaData          m_Data;

    //以下为Formula运行时参数
    signed char             m_scID;
    float                   m_fLastCalcValue;
} St_FormulaInfo;
typedef union
{
    struct
    {
        float x1;
        float x2;
        float x3;
        float x4;
        float y1;
        float y2;
        float y3;
        float y4;
    } fX;
    float f_Arry[8];
} UN_Formula_fx;

//typedef struct
//{
//	UN_Formula_fx fX;
//	unsigned char Refreshed[8];
//}ST_Formula_fX;

/*以下是通道CV管理相关结构定义*/
typedef struct //St_CvInfo
{
    St_CvData               m_CvData;

    //这里增加CV运行时参数
} St_CvInfo;

typedef struct //St_PulseInfo  Add by DKQ 08.20.2014
{
    St_PulseData          m_Data;

    //以下为Pulse运行时参数
    signed char           m_PulseID;

} St_PulseInfo;

/*以下是Schedule Buffer管理相关结构定义*/
typedef struct //St_StepBuf
{
    St_SMBBlockData  m_SMBBlockData;
} St_SMBBlockInfo;
typedef struct //St_StepBuf
{
    St_StepInfo             m_StepInfo;
    //St_SimulationInfo       m_SimulationInfo;      //12.22.2013ADD
    St_FormulaInfo          m_FormulaInfo;        //99%的用途一步之中只有1个formula,如果有多个formula需要运行时load
    St_PulseInfo            m_PulseInfo;
    St_CvInfo               m_CvInfo;
    St_SMBBlockInfo         m_SMBBlockInfo;
    MsTime                  m_StartTime;          //step start time

    //下面的变量暂时不实现。

} St_StepBuf;  //提供Step task使用
typedef struct //St_LimitInfo
{
    int                     m_SN;

    unsigned char      m_bAuxCheck:1;
    unsigned char      m_bAnyLimit:1;
    unsigned char      m_bReached:1;
    unsigned char      m_bChecked:1;
    int                m_AnyLimitReachedCount;    //这些变量虽然是下位机运行时变量，但是为了方便操作，写在协议文件里了。

} St_LimitInfo;
typedef struct //St_ChannelStep
{
    St_StepBuf              m_Info[2];            //两个Info，其中一个为Background Info，另一个为Front Info，两者切换使用

    unsigned char           m_ucActiveIdx;        //当前步Info的Index
    unsigned char           m_bIsDvInterval;      //用来标识本step是否只有一个Dv_time>x的loglimit适用，如果是这样，就要特殊处理
    unsigned char           m_bIsFirstLogPoint;   //标识是否第一个LogPoint;
    //unsigned char           m_bSdlEnabled;
    float                   m_fLogInterval;       //记录log interval
    unsigned long long      m_u64LogInterval;

    MsTime                  SysTimeStepTask;	//0819
    MsTime                  MsTimeStepTask;

    unsigned char           m_LimitCount;
    unsigned char           m_StepLimitCount;
    unsigned char           m_LogLimitCount;
    St_LimitInfo            m_limitInfo[IV_MAX_LIMIT_NUMBER];
    //	SysTime                 SysTimeLimitCheckTask;  //dirui add for Limit delay 0819
    MsTime                  MsTimeLimitCheckTask;    //dirui add for Limit delay
} St_ChannelStep;
typedef struct //St_ChannelSchedule
{
    St_MD5                  m_ID;

    MsTime                  m_StartTime;    //schedule start time;

    signed char             m_scBufIdx;
    unsigned char           m_bResetTestTime;

    St_ScheduleSetting      m_Settings;
    St_SchTestObject        m_TestObject;

    St_ScheduleItemCount    m_ItemCount;

    unsigned short          m_NextStepIdx;
    unsigned short          m_RunningStepIdx;

    //    SysTime    SysTimeScheduleTask;	0819
    MsTime    MsTimeScheduleTask;
} St_ChannelSchedule;
typedef struct
{
    unsigned char m_SafetySNIndex;
    unsigned char m_LimitSNIndex;
} ST_SNINDEX;
//#if SUPPORT_PARALLEL == 1
//
//Grouping information and settings:
//
typedef enum //通道模式
{
    //	eChannelMode_Normal = 0, // default. single channel。单通道运行
    eChannelMode_Master = 0, // master in parallel, defualt, even for single channel running status (group size is 1)。并联，作为主通道（group的第一个通道）。
    eChannelMode_Submaster, //interunit parallel, first slave channel in other-than-master unit。多unit并联时非主unit的第一个通道
    eChannelMode_Slave //slave in paralle, other than master and submaster。从通道
} En_ChannelMode;




typedef struct
{
    unsigned char m_Chn_Rank;    // =0 (Master); 1 (Sub Master) ; 2(Slave)
    unsigned char m_Group_Size;  //如果是master的话，自己的group size,以便用于分电流
    ST_UNIT_N_CHN m_Supervisor;  //该通道的上级通道; 如果是Master，上级通道就是自己。如果是SubMaster，上级通道就是master。如果是 slave，上级通道就是 Sub master。
    ST_UNIT_N_CHN m_SubMasterChannels[MAX_UNIT_PER_GROUP];  //如果是master的话，自己的sub master通道位置
    unsigned char m_SlaveChnnels [MAX_CHN_PER_UNIT];  //如果是submaster的话，自己下属的slave通道
} ST_EXTENDED_CHN_INFO;






typedef struct
{
    //	En_GroupMode	m_eGroupMode; //intra unit para/inter unit para/serial/booster。是什么模式的group
    unsigned char 	m_eGroupMode; //intra unit para/inter unit para/serial/booster。是什么模式的group
    En_ChannelMode m_eChannelMode; // channel is normal/master/submaster/slave通道运行模式
    unsigned char m_uMasterUnitIdx;//主通道所在unit号。Submaster report data需要， submaster监听CAN上的并联相关命令需要。
    unsigned char m_uMasterChanIdx;//主通道在unit内的通道号。以便等到组内最后一个通道来计算（小）组和，需要知道是哪个组的，主要针对unit内部并联情况，多unit并联，此值为0（第一个通道）.
    unsigned char m_uGroupChanCount;  //default 1. number of channels in the group。Group有几个通道。
} St_GroupingInfo;

//
// group data
//
typedef struct {

    bool		m_bGroupNewCodeSampled;

    bool		m_bGroupCurrentSampleFlag_SafetyCheck;
    bool		m_bGroupVoltageSampleFlag_SafetyCheck;

    bool		m_bGroupCurrentSampleFlag_LimitCheck;
    bool		m_bGroupVoltageSampleFlag_LimitCheck;

    //存放整组数据
    float 		m_fGroupCurrent;
    float 		m_fGroupVoltage;
    float 		m_fGroupPower;
    float 		m_fGroupCCapacity;
    float 		m_fGroupDCapacity;
    float 		m_fGroupCEnergy;
    float 		m_fGroupDEnergy;

    //	float 		m_fGroupCapapcityTime;

} St_GroupData;

enum
{
    FLAG_TYPE_PARALLEL_NONE = 0x0,
    FLAG_TYPE_PARALLEL_FRESHSAMPLECODE = 0x1,
    FLAG_TYPE_PARALLEL_SAFETYCHECK = 0x2,
    FLAG_TYPE_PARALLEL_LIMITCHECK = 0x4,
    FLAG_TYPE_PARALLEL_8 = 0x8,
    FLAG_TYPE_PARALLEL_WHATEVER = 0xFF
};  //use bit


//
//parallel group ctrl
//
typedef struct
{
    float m_fGroupChandIdt;
    float m_fGroupChandVdt;

    float m_fGroupChanCurrent;
    float m_fGroupChanVoltage;

    float m_fGroupChanCapacity;
    float m_fGroupChanEnergy;

} St_GroupDetalValue;


typedef struct {

    unsigned char 		m_bCurrentRangeParallelHigh;  //paralllel high or not? Tell slave to set to high or off, when master is current high.

    unsigned long			m_ulGroupChanCtrlCode[MAXCHANNELNO];   //DAC值

    unsigned char m_uGroupChanCtrlType[MAXCHANNELNO];
    float m_fGroupChanCtrlValue[MAXCHANNELNO];

    float m_fGroupChanCurrRampDelta[MAXCHANNELNO];
    float m_fGroupChanVoltRampDelta[MAXCHANNELNO];
    St_GroupDetalValue m_GroupDeltaValue[MAXCHANNELNO];

    unsigned char		m_ucEngineHandler[MAXCHANNELNO];

} St_GroupCtrl;


//多unit并联才会用到的小组数据存储：
typedef struct {
    //表示来的新数据//sub unit采样更新标志
    //set T when received corr. unit CAN pack's value
    //TODO: when to reset??
    bool m_bSubGroupChanVoltageUpdated;
    bool m_bSubGroupChanCurrentUpdated;
    bool m_bSubGroupChanCCapacityUpdated;
    bool m_bSubGroupChanDCapacityUpdated;
    bool m_bSubGroupChanCEnergyUpdated;
    bool m_bSubGroupChanDEnergyUpdated;

    float m_fSubGroupChanVoltage;  //在不出异常的情况下，认为主通道电压就是组电压 --> 用于异常检查
    float m_fSubGroupChanCurrent;
    float m_fSubGroupChanCCapacity;  //对于一般limit判断里使用的capacity，时间要求不高，使用current估计计算。
    float m_fSubGroupChanDCapacity;
    float m_fSubGroupChanCEnergy;
    float m_fSubGroupChanDEnergy;

} St_SubGroupData; //主要给多unit并联使用，存放每个subgroup的小组数据


typedef struct
{
    unsigned short m_uSubGroupUnitID;

    unsigned short m_uSubGroupCtrlType;

    float	m_fSubGroupCtrlValue;

    unsigned char m_ucCANCMDType;

    bool m_bSubGroupCtrlUpdated;

} St_SubGroupCtrl;

//unit里通道共属状态：
typedef enum
{
    //eUnitMode_Normal,
    //eUnitMode_Para_IntraUnit,
    eUnitMode_Para_Master,
    eUnitMode_Para_Submaster,
    eUnitMode_Para_Slave
} En_UnitChannelMode;

typedef struct
{
    bool m_bGroupCheckSafety;

} St_GroupStatusInfo;

typedef struct
{

    unsigned short m_uUnitID;

    //	En_GroupMode		m_eUnitGroupingMode; // normal, intra-parallel, inter-parallel
    //	unsigned char 	m_eUnitGroupingMode; // normal, intra-parallel, inter-parallel
    //	En_UnitMode 	m_eUnitMode;  //single channel running, intra-unit paralle, inter-unit parallel master unit, inter-unit paralle submaster unit.
    //	bool 					m_bMasterUnit; //bool
    //	unsigned char	m_uMasterUnitID;
    //	unsigned char 	m_uGroupUnitCount;
    ST_UNIT_GROUP_INFO  m_UnitGroupInfo;
    St_GroupStatusInfo	m_stGroupStatusInfo;
    bool				m_bSubGroupDataValid[MAXUNITNO]; // 对多unit并联，保存小组是否送来小组和更新。由CAN命令report data置T,整组求和完毕全部清0.
    //用于标识subgroup在换步后新来了有效数据
    //和单通道模式的ChannelInfo_Manager::ChannelInfo[uChanIdx].Current_Sampleflg 等价
    St_SubGroupData		m_stSubGroupData[MAXUNITNO];
    St_SubGroupCtrl		m_stSubGroupCtrl;

} St_GroupInfo;

typedef struct
{
    St_GroupInfo  m_GroupInfo[MAX_GROUP_PER_UNIT];
    unsigned char m_GroupCount;
    unsigned char m_MyUnitID;
    unsigned char m_InterMasterGroupID; //作为主unit时，属于哪个并联组
    unsigned char m_SubMasterGroupID;
    unsigned char m_ParaMode;
    unsigned char m_Series2BoosterUnitCounter;
} St_UnitInfo;

typedef struct
{
    ST_UNIT_N_CHN		m_SubMasterChannels;
    float           ControlVoltage;
    float           OutputVoltage;
    bool            IsNeedCheckRunState;//0:不需要检查,1:需要检查。
// 	bool            RunState;//0:停止,1:运行。
// 	MsTime          CheckPeriod;
} St_Serial2BoosterOutInfo;

typedef struct
{
    bool     m_bStartCtrl;
    float    m_MasterCurrent;
    MsTime   m_MaskCtrlTick;
} St_Para_FeedBackCtrl;
//#endif //SUPPORT_PARALLLEL
typedef struct //St_ChannelInfo
{
    bool    IsOpenClampRange;
    BYTE    clampHighControlType;
    BYTE    clampLowControlType;
    float   clampHigh;
    float   clampLow;
} St_ClampRange; //1712211 zyx
typedef struct //St_ChannelInfo
{
    //SDL:
	bool                    m_bNoNeedFineTune_flag;		//20190808 dyy
    bool                    m_bDynamicControl;
//     unsigned char           m_bNeedFilter;
    float                   m_fResumeTestTime;
    float                   m_fResumeStepTime;
    float                   m_fResumeTC_Timer1;//zyx 211101
    float                   m_fResumeTC_Timer2;
    float                   m_fResumeTC_Timer3;
    float                   m_fResumeTC_Timer4;	
    unsigned long           m_u32VoltageAdc;
    unsigned long           m_u32CurrentAdc;
    unsigned long           m_u32BuildInVolt[MAX_BUILDIN_2V_COUNT];
    unsigned char           m_bBuildInVoltNewCode[MAX_BUILDIN_2V_COUNT];
    unsigned long           m_u32BuildInVolt_Pulse[MAX_BUILDIN_2V_COUNT];
    unsigned char           m_bBuildInVoltNewCode_Pulse[MAX_BUILDIN_2V_COUNT];
    float                   m_fBuildInVolt_Pulse[MAX_BUILDIN_2V_COUNT];
    float                   m_fBulitInData[MAX_BUILTIN_COUNT];  //新增BUILTIN 辅助数据
    unsigned long           m_u32CurrentRampDelta;
    unsigned long           m_u32VotlageRampDelta;
    float                   m_fCurrentRampDelta;  //并联专用 20150717
    //SDL^

    float                   m_fNormalizedVoltage;
    float                   m_fNormalizedCurrent;

    unsigned long           m_u32DacValue;
    float                   Clamp_Value;
    unsigned char           Clamp_Direction;
    float                   m_fCurrent;
    float                   m_fVoltage;
    unsigned char           m_ucCurrentRange;
    unsigned char           m_ucVoltageRange;
    unsigned char 			m_ucSyncStop:1;			//温控箱随SCH同步停止标志
    unsigned char			m_ucNone:7;					//预留 reserved
    unsigned char           m_ucStatus;
    unsigned char           m_bScheduleRunning;
	unsigned char           m_bThirdParty_FineTuningDone;  //190917,hpq
	unsigned char           m_bThirdParty_FineTuning;  //190917,hpq
	float                   m_fThirdParty_SetCurrent;  //190917,hpq
	float                   m_fThirdParty_FineTuningLastOffset;  //190917,hpq
	MsTime                  m_tLastFineTuningTime;     //190917,hpq

    unsigned long           m_u32RelayStatus;

    St_IndependantCtrl      m_IndependantCtrl;

    float                   m_fCurrentRangeOfPid; //1904031 zyx
    float                   m_fVoltageRangeOfPid;
    ///////////////////////add for schedule manager

    unsigned char   m_CtrlState;
    unsigned char   ScheduleStatus;
    unsigned char   StepStatus;
    unsigned char   m_UnsafeType;
    unsigned long int   m_SafetyStatus;
    unsigned long int   m_AUXSafetyStatus;  //dirui add 20131120
    unsigned char 	StepLimitProcess;  //dirui add 20130122
    unsigned char   LogLimitProcess;  //dirui add 20130122
    unsigned char   m_Event_Source;		//dirui add 0917
    unsigned char   Current_Sampleflg;  //dirui add 0917
    unsigned char   Voltage_Sampleflg;	//dirui add 0917
    unsigned char   m_bCVstage;         //hepeiqing add 20140327,in CV stage? for CCCV step


    WORD   Report_LimitID;     //dirui add 2013 0924
    WORD   Report_EquationID;  //dirui add 20130924

    unsigned char   EngineStatus;       //引擎状态  XXXXXX Error running    1代表 有效    0无效    comment out chen 20130123
    unsigned char   StartedReportNumber;
    unsigned char   StepTransationToStatus;
    unsigned char   DigitCtrl;
    unsigned char   Adjustment;
    unsigned char   DacReverse;
    unsigned char   SpecialProcess;
    unsigned long   ResumeTime;

    unsigned char   bNeedDacOutput;    //20130925,add by hepeiqing,Control Dac Output frequency
// 	unsigned char   bDigitalI;    //20131118 add    数字电流
    unsigned char   bDigitalV;

    unsigned char ScheduleUnsafeFlg;
    unsigned char chn_Running_Flag;    //idle sch or 第三方    comment out chen 20130513

    //float           m_fCapacity;   //  电池额定容量
    //float           m_fMass;
    //float           m_fSpecificCapacity;
    float           m_fVclampHigh;
    float           m_fVclampLow;
    float           m_fCurrentSimulationVclampHigh;      //09.25.2017 add by zc
    float           m_fCurrentSimulationVclampLow;				//09.25.2017 add by zc
    float           m_fPowerSimulationVclampHigh;      //10.04.2017 add by zc
    float           m_fPowerSimulationVclampLow;				//10.04.2017 add by zc
    //float           m_fNominalVoltage;  // 标(额)定电压
    BYTE               m_DataType_DigiVclampHigh;               //171016 hpq
    BYTE               m_DataType_DigiVclampLow;                //171016 hpq
    St_ClampRange   m_CurrentClampRange;    //1712211 zyx
    St_ClampRange   m_PowerClampRange;  //1712211 zyx
	St_SafetyRedundantVotlage   m_RedundantVotlage;  // 20190723 yy

    unsigned char chn_State;    //引擎转换状态    add by chen
    unsigned char   m_bCaliBoard;
    MsTime          m_LastCaliTime;    //防止校准时间过长，因为一般的校准只会进行几秒钟

    St_SimuRunInfo  m_SimuInfo;
    St_CvRunInfo    m_CvInfo;

    //AUX 添加20131120
    // 	unsigned char  IVCANID;
    // 	unsigned short AUXTYPE;     //该通道含有多少种辅助通道，按照bit表示
    // 	unsigned char AuxTypeChannelCount[MAXAUXTYPE];  //各种辅助通道的数量
    ST_SNINDEX m_AUX_SN_Index;
    //#if( SUPPORT_PARALLEL == 1)
    //	St_GroupingInfo		m_GroupingInfo;				//并联信息
    unsigned char       m_GroupID;
    St_GroupCtrl 		m_GroupCtrl;			//并联控制
    St_GroupData		m_GroupData;			//并联数据
    ST_EXTENDED_CHN_INFO  Extended_Chn_Info;
    //#endif

    //St_Grp    m_GrpInfo4SDL;
} St_ChannelInfo;



typedef struct //St_ScheduleBuffer
{
    unsigned char LimitCount;
    unsigned char IfAnyLimitCount;

    unsigned short StepID;
} St_AuxLimitStatus;


typedef struct //St_ScheduleBuffer
{
    St_MD5               m_ID;

    MsTime               m_AbandonTime;

    unsigned char        m_bValid;
    unsigned char        m_ucReserved1[3];

    unsigned long        m_u32IvRegister;      //最多支持32个通道注册
    //unsigned char        m_ucIvCount;        //标示有N个IV通道在使用这个Schedule;  //使用bit表示一个通道是否register
    unsigned char        m_ucMacCurrentRange;  //本scheduel中最大的电流量程

    //以上为运行时参数，不参与内存拷贝

    St_ScheduleSetting   m_Setting;
//	St_SchTestObject     m_TestObject;//171113 hpq add for pro8

    St_ScheduleItemCount m_ItemCount;

    St_StepData          m_StepData[IV_MAX_SCHEDULE_STEP_NUMBER];
    St_StepData        m_StepData_ExtraIR;  //170721,PeiqingHe,重要！important!此行代码不能被删除！
    St_FormulaData       m_FormulaData[IV_MAX_SCHEDULE_FORMULA_NUMBER];
	St_AdvFormula       m_AdvFormula[MAX_ADVFORMULA_COUNT];	
    St_PulseData         m_PulseData[IV_MAX_PULSE_BUFFER_NUMBER];
    St_TestSetting       m_TestSetting[MAX_TEST_SETTING_COUNT];
    St_CvData            m_CvData[IV_MAX_SCHEDULE_CV_NUMBER];
    St_SMBBlockData      m_SMBBlockData[MAX_SMB_BLOCK_CONTENT_COUNT];
} St_ScheduleBuffer;

/*****Simualtion Buffer******/
// typedef struct //St_SimulationBuffer
// {
// 	St_MD5               m_ID;
// 	MsTime               m_AbandonTime;
// 	unsigned char        m_bValid;
// 	unsigned char        m_bIsBatterySimu;       //是否为Battery Simulation
// 	signed char          m_scSimuBufferIdx;
// 	unsigned long        m_u32IvRegister;        //最多支持32个通道注册
// 	unsigned long        m_u32PointNumber;       //此为本buffer的点数目
// 	Un_SimuPoints      	 m_Data;
// }St_SimulationBuffer; //12.22.2013ADD


/*以下是Pulse Buffer管理相关结构定义*/
typedef struct
{
    St_MD5               m_ID;

    MsTime               m_AbandonTime;

    unsigned char        m_bValid;
    //unsigned char        m_ucIvCount;            //标示有N个IV通道在使用这个Schedule;
    unsigned char        m_ucReserved1[3];

    unsigned long        m_u32IvRegister;        //最多支持32个通道注册
    //以上为运行时参数，不参与内存拷贝

    St_PulseData         m_Data;
} St_PulseBuffer;

/*以下是config管理相关结构定义*/
typedef struct //St_ConverterCharacter
{
    unsigned long       m_u32Mid;
    unsigned long       m_u32Max;
} St_ConverterCharacter;
typedef struct //St_BoardCFG
{
    St_BoardGlobalCFG   m_Global;

    St_ChannelCFG       m_Channel[MAXCHANNELNO];

    //以下为本地初始化内容：

    unsigned char chn_Running_Flag2;
} St_BoardCFG;

typedef struct
{   unsigned char  		m_valid;   //该板是否有效标记
    ST_ABI_GLOBLE 			m_abi_globe;   //全局信息
    ST_ABI_CHN 				m_abi_chn[32];  //最大128个,考虑到Addon板一个就有32个，电池包有96个
    ST_ABI_ASSEMBLE  	m_assemble;  //板在机箱中的组合信息
    unsigned short     m_RSN;     //板在AUXMC中的注册信息
} ST_BUILTIN_BOARD_INFO_IN_EEPROM;

typedef struct
{
    unsigned char  		m_valid;   //该板是否有效标记
    ST_ABI_ASSEMBLE    m_assemble;  //板在机箱中的组合信息
    ST_ABI_GLOBLE 			m_abi_globe;   //全局信息
    unsigned short     m_RSN;     //板在AUXMC中的注册信息
    unsigned char m_Start_InUnit; // Add Nov 25, 2013  用于存储排序后该板内通道在辅助unit内的通道序号
    unsigned char m_End_InUnit;		//Add Nov. 25,2013

} ST_AUX_BOARD_INFO_SAVE;    //18字节

typedef struct //St_WindowFilter
{
    unsigned long                                                m_uBuf[IV_SAMP_VOLTAGE_FILTER_DEPTH];
    unsigned short                                               m_uHandler;

    unsigned short                                               m_uFilterDepth;

    unsigned long int                                            m_u32Sum;

    unsigned long int                                            m_u32LastValue;
} St_WindowFilter;
/*History Data Struct*/
typedef struct //St_HistoryData
{
    //滤波深度可动态配置，增加接口
    //CIRCULAR_QUEUE(unsigned long,IV_SAMP_CURRENT_FILTER_DEPTH)   m_Current;    //电流历史采样数据 循环队列
    // CIRCULAR_QUEUE(unsigned long,IV_SAMP_VOLTAGE_FILTER_DEPTH)   m_Voltage;    //电压历史采样数据 循环队列

    unsigned long                                                m_uCurrBuf2[IV_SAMP_CURRENT_FILTER_DEPTH];
    unsigned long                                                m_uVoltBuf2[IV_SAMP_VOLTAGE_FILTER_DEPTH];
    unsigned short                                               m_uCurrHnd;
    unsigned short                                               m_uVoltHnd;

    unsigned short                                               m_uCurrFilterDepth;
    unsigned short                                               m_uVoltFilterDepth;

    unsigned long int                                            m_u32CurrentSum;
    unsigned long int                                            m_u32VoltageSum;

    unsigned long int                                            m_u32LastCurrentValue;
    unsigned long int                                            m_u32LastVoltageValue;

    St_WindowFilter                                              m_BuildInVolt[MAX_BUILDIN_2V_COUNT];
} St_HistoryData;

typedef struct //St_HistoryData
{
    unsigned long                                                m_uBuf2[IV_SAMP_CURRENT_FILTER_DEPTH];
    unsigned short                                               m_uHnd;

    unsigned short                                               m_uFilterDepth;

    unsigned long int                                            m_u32Sum;

    unsigned long int                                            m_u32LastValue;
} St_HistoryData_ACR;

/*Message Define For Tasks*/
typedef struct //St_FramMessage
{
    unsigned char   m_bIsWrite;
    unsigned char   m_bPID;       //是否是PID操作
    unsigned short  m_u16ChannelIdx;
#ifdef REPORT_FRAM_EVENT
    WORD               m_wChannelBitIdx;
    WORD               m_wChannelCount;
    WORD               m_wFramType;
    MsTime             m_MsTime;
#endif
    unsigned short  m_u16Length;
    unsigned long   m_StartAddress;
} St_FramMessage;

typedef struct
{
    unsigned char LimitPropertyChange;
    unsigned short LimitID;
    unsigned short EquationID;
} St_ScheduleRunMsg;
typedef union
{
    St_Assign      m_Assign;
    St_ScheduleRunMsg m_RunMsg;

    //Di rui: add other schedule message here;

} Un_ScheduleMessage;
typedef struct
{
    unsigned char MsgPriority;
    unsigned char MsgNumber;
    //unsigned char LimitArriveNumber;
    unsigned short StepID;
    unsigned short FromStepID;
    unsigned long  CycleID;
    unsigned long  FromCycleID;  // 新增
    unsigned char  ForceJumpFlag;   // 新增
    //unsigned short LimitID;
    //unsigned short EquationID;
    unsigned short GotoStepID;
    //unsigned char LimitPropertyChange;
    //...
    Un_ScheduleMessage  m_Message;
    unsigned char m_btCMD;
    ST_EVENT_SCHEDULE_RUNNING st_status_sch_running;
} St_ScheduleMessage;

typedef struct
{
    unsigned char MsgNumber;

    unsigned char StepID;
    unsigned short ToStepID;
    unsigned short FromStepID;
//	unsigned short CircleID;    dirui del 2018/12/18

    unsigned short LimtID;
    unsigned short EquationID;
    //...
} St_StepMessage;  //最终要统一到大数据体

// typedef struct //St_TaskMessages           dirui del 2018/12/18
// {
// 	CIRCULAR_QUEUE(St_FramMessage,IV_FRAM_MESSAGE_DEPTH)            m_Fram;
// 	CIRCULAR_QUEUE(St_ScheduleMessage,IV_SCHEDULE_MESSAGE_DEPTH)    m_Schedule;

// 	//add other task message here;
// }St_TaskMessages;

typedef struct
{
    float         m_fHigh;
    float         m_fLow;

    unsigned char m_bIfCheck;
} St_SaftyCheckItem;

typedef struct
{
    St_SaftyCheckItem       checkSYS_V;
    St_SaftyCheckItem       checkSYS_I;
    St_SaftyCheckItem       checkSYS_P;
    St_SaftyCheckItem       checkSCH_V;
    St_SaftyCheckItem       checkSCH_I;
    St_SaftyCheckItem       checkSCH_P;
    St_SaftyCheckItem       checkSCH_C; //step Capacity check zyx 180521
    St_SaftyCheckItem       checkSCH_t; //step time check zyx 180521
} St_Safty;
typedef struct
{
    unsigned char   m_bIfCheckFlag;     //0:不需要检查，  1：电压  2：温度
    St_SaftyCheckItem       checkSCH_Aux_V;
    St_SaftyCheckItem       checkSCH_Aux_T;
} St_BuiltinSafety;

typedef struct
{
    unsigned char   m_ChannelIdx;
    unsigned char   m_LimitIdx;
    unsigned short  m_StepIdx;
    St_MD5          m_SchMd5;
} St_LimitPointer;

typedef struct
{
    float           m_fStartTime;
    float           m_fMinError;
    float           m_fStep;
    float           m_fMaxAdjust;
	float           m_fParaWaitTime;  // add by qjm  增加并联时等待从通道数据更新的时间， 按128滤波深度计算
	float           m_f_CurCoarseTuneStartTime;
} St_CurFineTuneFactor;

typedef struct
{
    MsTime          m_OutputInterval;// fine tune output interval, current output would be shock if fine tune control too quick.
    float           m_LastOutput;
    float           m_LastSetValue;
    float           m_SampleValue;
    float           m_TotalAdjust;
    float           m_Err;

    float           m_fMinError;       //微调启动的最小误差
    float           m_fStep;           //每次微调的幅度
    float           m_fMaxAdjust;      //最大微调幅度
    float           m_fSetValue;
} St_CurFineTune;   //Add by DKQ on 06.03.2014 for current fine tunning

// typedef struct                   dirui del 2017/12/18
// {
// 	float           m_LastOutput;
// 	float           m_LastCurrentSet;
// 	float           m_LastLastCurrentSet;
// 	float           m_LastActuCurrent;
// 	float           m_TotalAdjust;
// 	float           m_Err;
// 	float           m_LastErr;
// 	float           m_LastVoltage;
// } St_PowerFineTune;   //Add by DKQ on 06.04.2014 for Power fine tunning


typedef struct
{
//	bool           bNeedCheck;
//	bool           bGoStop;
    unsigned char  CheckType;
    float          fCtrlValue;
    float          fError;
    bool           bCtrlCurrClampV;
    float          fClampVerror;

    signed long    Counter;
    unsigned char  ErrorCode;
    bool           bWarning;
} St_CtrlValueCheck;

typedef struct
{
    MsTime         NextTime;    //上次检查控制值的时间
    bool           bChecking;
    unsigned char  ChannelIdx;

    float          fErrorFactor;
} St_CtrlValueSetting;

typedef struct
{
    bool           bNeedCheck;
// 	bool           bGoStop;     //如果CtrlValue异常了，就停止
// 	float          fWarningCapa;
// 	float          fErrorCapa;
// 	float          fWarningEnergy;
// 	float          fErrorEnergy;

    float          fErrorVoltage;
    signed long    Counter;
    signed long    MaxCount; //Max times of Battery Voltage lower than threshold.
    unsigned char  ErrorCode;
    //bool           bWarning;
} St_IrregularCheck; //zyx 180705
typedef struct
{
    MsTime         NextTime;    //上次检查合理性的时间
    bool           bChecking;
    unsigned char  ChannelIdx;
} St_IrregularSetting;

typedef struct
{
    unsigned char uc_start;
    unsigned char uc_length;
    //unsigned char uc_N;    //系数，暂时先这样吧，因为不能规定好每个索引对应的类型  开始初始化把N系数设为0
    unsigned long ul_data[MAX_ADC_CODE_QUE];
} st_ADC_Code_Que;

typedef struct
{
    unsigned char m_IV_unitID;
    unsigned char m_AuxChnIdx;
    unsigned char m_AuxType;
    unsigned char m_AuxSensor;
    unsigned char m_IVChnIdx;
    unsigned char m_RptDest;  //预留
    unsigned char m_Reserve[2];
    float         m_value;
    MsTime    TrigTime;
} ST_BUILTIN_AUX_DATA_TRIG;

typedef struct
{
    unsigned char m_IVCh_Index;     // 在该unit内的IV通道序号
    St_OneAuxChInfo  m_AuxInfo_In_OneIVCh[MAX_MAP_COUNT_PER_UNIT];  // 一个IV通道分到的辅助通道  // #define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT
    unsigned short m_Total_In_OneIVCh[MAXAUXTYPE];  // 同种辅助类型下通道总数量  	//--new add---新增加
		unsigned short m_aux_type_ptr[MAXAUXTYPE];
} St_PC2IVUnit4AuxMap;  //  From PC to IVUnit for (about) AuxMap

#if DEBUG_TRACE_RECORD == 1
typedef struct
{
    unsigned short   m_u16RecordType;
    MsTime           m_RecordTime;
    //	SysTime          m_sysTime;	0819

    unsigned long    m_u32Parameter1;
    unsigned long    m_u32Parameter2;
    unsigned long    m_u32Parameter3;
    unsigned long    m_u32Parameter4;

    float            m_fParameter1;
    float            m_fParameter2;
} St_DebugPoint;

typedef CIRCULAR_QUEUE(St_DebugPoint,DEBUG_TRACE_RECORD_DEPTH) St_DebugTraceRecord;
#endif

