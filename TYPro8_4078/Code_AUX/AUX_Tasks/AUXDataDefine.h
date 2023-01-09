#pragma once
#include "../../COMMON_os/Task_Dispatcher/OS_ServeProcess.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../common_OS/protocols/CAN_Hwf.h"
///// IVMCU 一级的定义
#define MAX_IVMCU_PER_SYS   MAX_IVUNIT   //限定一个系统最多16个IVMCU
//#define MAX_IVCH_PER_IVMCU  16  //限定一个IVMCU最多只带8个IV通道，简化版辅助，但对ARPRE 需设为16，实际使用第15个通道
#define MAX_IVCH_PER_IVMCU  MAXCHANNELNO  //限定一个IVMCU最多只带8个IV通道，简化版辅助，但对ARPRE 需设为16，实际使用第15个通道
#define MAX_LIMIT_PER_IVCH   16 //规定每个IV通道最多有16个limit
#define MAX_IFANY_LIMIT_PER_IVCH	16  //规定每个IV通道最多只有16个IFANY limit
#define MAX_DATA_GROUP             9  // 规定最多8个数据组。一般而言一个数据组对应于一个支持16通道的ADC
#define MAX_AUXCH_PER_DATA_GROUP        16 // 
#define MAX_AUXMCU_PER_IVMCU 6   //简化版辅助
//#define AUX_ADC_MAX_CHN       16
////  辅助MCU一级的定义
#define MAX_AUX_TYPE     MAXAUXTYPE //暂定最多有16种辅助通道类型
//#define MAX_AUX_TYPE_BOARD                      16
//#define MAXAUXTYPE							(8)//(NUM_OF_AUX_CHAN_TYPE)
#define MAX_SAFETY_RANGE					2

//AO存放位置放到96以外，在databank里放到group3的位置，没有超96.只改存储最大值为128.group0放AI,group1放DI，group2放DO，group3放的AO.
#define MAX_AUXCH_PER_BOARD  128    //17 		//一个辅助板上最多16个通道 额外支持温度17通道
#define MAX_HUMIDITY_PER_BOARD	4		//一个辅助板上最多有4个湿度通道（即四个串口）
#define MAX_ADCNUM_PER_BOARD  2   	//一个板子上最多2个ADC
//#define MAX_AUXCH_PER_AUXMCU  (MAX_AUXCH_PER_BOARD * MAX_BOARD_PER_AUXMCU) //一个辅助MCU上最多128个辅助通道，它们可以是同一类型，也可以是不同类型
#define MAX_AUXCH_PER_AUXMCU	MAX_CH_IN_AUX_UNIT

#define MAX_FACE_PER_BOARD	 2;   // 暂时定有副板时，总面板数为两块板。

#define MAX_AUX_CHANNEL_NUM_PER_ADC 16
#define MAX_ADC_CHIPS   16

#define MAX_DIDO_PIN_PER_BOARD       32
#define MAX_ADC_CODE_QUE             32
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


#define    MAX_AUX_TRIGGROUPLOG_POINT  32  // St_SDL_OnePoint points[32]

enum
{
    NOUSE=0,    //没有用
    BEUSED=1,    //实际使用
    VIRTUAL=2,   //虚拟
};
enum
{
    HIGH_COMMON_MODE=1,
    LOW_COMMON_MODE,
    TEMPERATURE_MODE,
    BATTERYCELLS,
    MULTIAUX,
};
enum
{
    SAMPLEIDLE=0,
    SAMPLEISR,
    SAMPLETASK,
};
enum
{
    AUXADC_IDLE,
    AUXADC_NEW_CONVERT=1,
    AUXADC_READ_LAST_CONVERT,
    AUXADC_WAIT,
};



typedef enum
{
    AUX_Sample_IDLE,
    AUX_Sample_TRIGGER,
    AUX_Sample_READ,
    AUX_Sample_WAIT,

} AUX_Sample_status;

typedef enum
{
    AUX_2ndV_LC_Sample_IDLE,
    AUX_2ndV_LC_Sample_SWITCH,
    AUX_2ndV_LC_Sample_WAIT1,
    AUX_2ndV_LC_Sample_TRIGGER,
    AUX_2ndV_LC_Sample_WAIT2,
    AUX_2ndV_LC_Sample_READ,

} AUX_2ndV_LC_Sample_status;

typedef union
{
    unsigned char m_ECODE1:1;
    unsigned char m_ECODE2:1;
    unsigned char m_ECODE3:1;
    unsigned char m_EOE:1;
    unsigned char m_Reserved:4;
    unsigned char m_Value;
} UN_AUX_ADC_CS_ADDRESS;


typedef struct
{
    unsigned char uc_start;
    unsigned char uc_length;
    //unsigned char uc_N;    //系数，暂时先这样吧，因为不能规定好每个索引对应的类型  开始初始化把N系数设为0
    unsigned long ul_data[MAX_ADC_CODE_QUE];
} st_ADC_Code_Que;

typedef struct
{
    bool m_Valid;                               // 用于标志有效的片选地址
    bool b_ADCCH_Valid[17] ;                 //每个ADC最多16个通道，考虑LTC2498 内部有一个温度通道，总共17个通道
    bool b_Finished;                         //该该ADC芯片的通道已经转换完成
    bool b_Triggered;
    unsigned char  m_BoardType;                 //记住板型，对于数据存放可能有效
    UN_AUX_ADC_CS_ADDRESS  m_CS_Address;                //片选地址的编码
    unsigned short m_Wait_Count;                //该cs对应的板上ADC的转换时间
    unsigned char  m_WorkingChn;                //该CS对应的板上ADC的正在工作的通道
    unsigned char  sspChn;                       //ssp chn
    unsigned char  m_StateMachine;                 //内部状态机
    unsigned short  counter;                 //每个ADC芯片当前的计数器
    unsigned char  m_ADCCH_MaxCnt;              //每个ADC芯片实际支持的通道总数。
    unsigned long (* m_BoardADC_Trigger)     (unsigned char u_ADCid,unsigned char ADCchn);   //该CS对应的板上ADC的trigger函数指针
    unsigned long (* m_BoardADC_Rd)        (unsigned char u_ADCid,unsigned char ADCchn);   //该CS对应的板上ADC的Rd函数指针
    unsigned char m_data_group_idx;        //用于表明该片选对应ADC及ADC们对应的data group 的起始index
    //相当于过去的虚拟板的概论，但不叫虚拟板了，以避免和板采样直接关联
    //对于普通辅助通道版 板ID就是 group_idx
    // 对于高共模， group_idx =0, 只有一个group
    // 对于 Addon, group_idx=0 对应于2449， group_idx =1 对应于 2498
    //对于 电池包 group_idx 0 -5 对应于1-96个通道 （ADC1-8），group_idx 6-7 对应于97-128 （ADC 9-11），

} ST_AUX_ADC_CS;


enum TE_CHECK_T_VOLTAGERANG
{
    COUNT_TEMPERATURE = 0,
    CHECK_T_VOLTAGERANG ,
};
//内部维护状态机状态，具体参见流程图
typedef enum
{
    // 	AUX_SAMPLE_IDLE_ENUM = 0xC0,
    // 	AUX_SAMPLE_WAITTING_TRIGGER,
    // 	AUX_SAMPLE_WAITTING_SAMPLE,
    // 	AUX_SAMPLE_TRIGGER_NEXT,
    // 	AUX_SAMPLE_WAITTING_TRIGGER_NEXT,

    // 	AUX_SAMPLE_RESERVE1,
    // 	AUX_SAMPLE_RESERVE2,
    AUX_SAMPLE_IDLE_ENUM = 0xC0,
    AUX_SAMPLE_WAITTING_TRIGGER,
    AUX_SAMPLE_WAITTING_SAMPLE,
    AUX_SAMPLE_TRIGGER_NEXT,
    AUX_SAMPLE_WAITTING_TRIGGER_NEXT,
    AUX_SAMPLE_WAITTING_POLL0,    //add by qjm20141128
    AUX_SAMPLE_WAITTING_POLL1,
    AUX_SAMPLE_WAITTING_POLL2,
    AUX_SAMPLE_WAITTING_POLL3,
    AUX_SAMPLE_WAITTING_READ,    //add by qjm20141128
    AUX_SAMPLE_PROCESSING_DATA, //add by qjm20141208

    AUX_SAMPLE_RESERVE1,
    AUX_SAMPLE_RESERVE2,
} AUX_SAMPLE_STATE;


typedef struct
{
    unsigned char BoardID;
    unsigned char ADCID;
    unsigned char ChannelID;
} ST_AUXSAMPLE;
typedef struct
{
    unsigned char m_Unit_ID;
    unsigned char m_Auxtype;
    unsigned char m_AuxChannelFromID;
    unsigned char m_AuxChannelToID;
} ST_AUXSYSTEMCFG; //Systemconfig配置

typedef struct
{
    unsigned char SSPNum;
    //ssp 参数结构
} ST_SSP; //SSP 结构定义

typedef union
{
    ST_SSP  m_SSP;
    //ST_GPIO m_Gpio;
} UN_DRIVERCFG; //配置各种类型的驱动

typedef struct
{
    unsigned char m_DriverType;
    UN_DRIVERCFG  m_DriverCfg;
} ST_HARDWARECFG; //不同类型板的配置

typedef struct
{
    float gain;
    float offset;

    //others  或许这里与IV应该同步？？
} ST_AUXCANNELCFG;
typedef struct
{
    //板信息包括:
    //1、板类型
    //2、通道数量
    //3、总线的类型(驱动信息)
    //4、校准信息

    unsigned char m_BoardType;
    unsigned char m_ChannelCount;
    ST_HARDWARECFG m_HardWareCfg;
    //ST_AUXCANNELCFG    辅助通道校准等配置信息
    //ST_AUXCANNELCFG m_Channel[AUX_ADC_MAX_CHN];    //校准信息？  20131202

} ST_AUXBOARDCFG; //辅助板级配置信息 放置于EEPROM中




typedef struct
{
    unsigned char  		m_valid;   //该板是否有效标记
    ST_ABI_ASSEMBLE    m_assemble;  //板在机箱中的组合信息
    ST_ABI_GLOBLE 			m_abi_globe;   //全局信息
    unsigned short     m_RSN;     //板在AUXMC中的注册信息
    unsigned char m_Start_InUnit; // Add Nov 25, 2013  用于存储排序后该板内通道在辅助unit内的通道序号
    unsigned char m_End_InUnit;		//Add Nov. 25,2013

} ST_AUX_BOARD_INFO_SAVE;    //18字节


typedef struct
{
    unsigned char     m_ActualCounts;  //该辅助类型在该IV通道下的辅助实际通道数
	unsigned char	  m_Counts_ByScheduleMap;	//该辅助类型在IV通道 schedule里面实际的MAP数量
    St_MAP_AuxCh2IVCh m_MapInfo[MAX_AUXCH_PER_AUXMCU];
} ST_MAP_FROM_PC;  //用于存储从PC下来的有关某IV通道的各类辅助通道的配置信息

typedef struct
{   unsigned char m_valid;  //用于标记此辅助通道是否真的被用过
    unsigned char m_IVUnit; //unit ID
    unsigned char m_IVCh;
    unsigned char m_AuxType;
    unsigned char m_Index_InIVCh;       // 该辅助类型的通道在该IV通道里的索引序号
    unsigned char m_Index_InAuxUnit;    // 该辅助类型在其辅助unit内通道号
} ST_DOWNLOAD_MAP_FROM_PC;



typedef struct
{
    bool            m_bValid;
    bool		        m_bOut;
    unsigned char   m_BID;
    unsigned char   m_CID;
} ST_Aux_oneDOCtrl;
typedef struct
{
    unsigned char 	 m_DoCount;
    ST_Aux_oneDOCtrl m_DOValue[MAX_DIDO_PIN_PER_BOARD];
} ST_AUX_DOCtrl;








typedef struct
{   unsigned char  		m_valid;   //该板是否有效标记
    ST_ABI_GLOBLE 			m_abi_globe;   //全局信息
    ST_ABI_CHN 				m_abi_chn[MAX_CH_IN_AUX_UNIT];  //最大128个,考虑到Addon板一个就有32个，电池包有96个
    ST_ABI_CHN         m_abi_uart_chn [8]; // 暂定最大8个串口通道。注意是指数值通道，不是指8个物理UART。
    ST_ABI_CHN         m_abi_DAC_chn[8];  //暂定最大8个DAC通道
    ST_ABI_ASSEMBLE  	m_assemble;  //板在机箱中的组合信息
    unsigned short     m_RSN;     //板在AUXMC中的注册信息
}
ST_AUX_BOARD_INFO_IN_EEPROM;

typedef struct
{

    ST_AUX_BOARD_INFO_IN_EEPROM   m_BoardInfo;
    unsigned char    m_data_group_start; //该板的对应的起始 data_group_index;
    unsigned char    m_TotalADC;
    unsigned char    m_Start_InUnit; // Add Nov 25, 2013  用于存储排序后该板内通道在辅助unit内的通道序号
    unsigned char    m_End_InUnit;		//Add Nov. 25,2013
    //一下是为了保存以前的结构，使代码能调过。以后删除
    //ST_AUX_BOARD_INFO_SAVE   m_BoardInfo;
    Un_AUXCHINFO  m_AuxSensor_ChInfo[MAX_CH_IN_AUX_UNIT];   //02. 24.2014
} ST_AUX_BOARD_INFO;


typedef struct
{
    unsigned char m_StartInSnake;      // 在长蛇中的起始位置
    unsigned char m_TotalChnNum;      //某类型辅助通道的总数； 不会大于128

} ST_AUX_IN_UNIT_LOOKUP_TABLE;

typedef struct
{   unsigned char  m_valid;
    unsigned char  m_data_group_idx;   //
    unsigned char  m_chn;   //Electric Channel In  电气通道序号
} ST_AUX_CHN_LOCATION;   //辅助通道在数据bank中的的位置结构


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
    float f_gain;
    float f_offset;
    float f_extra;  // Add by DKQ 03.26.2014

    //
} ST_AUX_CONFIG_FOR_SAMPLE;   //chen add 20131213




typedef struct
{
    ST_ABI_CHN     		m_Chn_Info;  //该通道的特性参数
    st_ADC_Code_Que  	aux_adc_history; //ADC 历史码值
    unsigned long 		aux_instant_code;  //瞬时ADC码值
    unsigned long 		aux_avg_code;  //平均ADC码值

    float 						aux_avg_voltage;	//平均后的采样电压数据值
    float 						aux_pv_Valtage;  //keep the old one, for the time being
// 	float 						aux_instant_data;
    float 						aux_data;			//转换后的物理值数据
    float 						aux_dx_dt;		//对时间的一阶导数
    float 						aux_d2x_dt2;  //对时间的二阶导数
    bool  						updated;			//采样是否更新
    MsTime            value_update_time;  //160429

    St_AUX_VALUE   		updated_aux_data;
//    St_AUX_VALUE   		updated_aux_dxdt;
//    St_AUX_VALUE   		updated_aux_d2xdt2;
    unsigned char     aux_data_error_count;
} ST_AUX_DATA;					//用于存放与ADC相关的辅助的数据

typedef struct
{
    bool 							m_valid;
    TE_AUX_CHAN_TYPE   m_AuxType;
    unsigned char   aux_data;  //状态值
    bool  updated;			//采样是否更新

} ST_AUX_DIDO_DATA; //用于存储与ADC无关的辅助的数据，比如digital inupt digital output等


typedef struct
{
    st_ADC_Code_Que  InChipTemp_adc_history; //ADC 历史码 值
    unsigned long InChipTemp_instant_code;  //瞬时ADC码值
    unsigned long InChipTemp_avg_code;      //平均ADC码值
    float InChipTemp_avg_voltage;	//平均后的采样电压数据值
    float InChipTemp_data;			//转换后的物理值数据
    bool  updated;			//采样是否更新
} ST_INCHIP_TEMP;

typedef struct
{
    WORD          m_limit_SN;  //limit 序列号
    unsigned char m_Source_CANID;  //源 CANID以便回复时用
    unsigned char m_IV_index;  //IV通道号
    unsigned char m_aux_type;		//辅助类型
    unsigned char m_chn_index;  //同一辅助类型的通道号
    unsigned char m_left_type;  //0=原始数据；1=一阶导数， 2= 二阶导数
    unsigned char m_ComparsionSign; //0:>; 1<; 2 >=; 3<=  参考IV部分
    float 		  m_right_value;
    unsigned char m_limit_Reached;
    unsigned char m_limit_alive;     //标记该limit是否已被清除失效
    unsigned char m_FirstLimit;
} ST_AUX_LIMIT;

typedef struct {
    unsigned char chn_1:1;
    unsigned char chn_2:1;
    unsigned char chn_3:1;
    unsigned char chn_4:1;
    unsigned char chn_5:1;
    unsigned char chn_6:1;
    unsigned char chn_7:1;
    unsigned char chn_8:1;
} ST_CHN2BIT_MAP;

typedef struct
{
    WORD 	        m_limit_SN;  					//limit 序列号
    unsigned char m_Source_CANID;  //源 CANID以便回复时用
    unsigned char m_IV_index;  	//IV通道号
    unsigned char m_aux_type;		//辅助类型
    unsigned char m_left_type;  //0=原始数据；1=一阶导数， 2= 二阶导数
    unsigned char m_ComparsionSign; //0:>; 1<; 2 >=; 3<=  参考IV部分
    float 				m_right_value;
    unsigned long long m_limit_Reached[2];  // 用共128个bit。每个bit来记录每个通道的limit_Reached的变化情况。
    unsigned char m_bReached;
    unsigned char m_limit_alive;     //标记该limit是否已被清除失效
    unsigned char m_FirstLimit;
} ST_AUX_IFANY_LIMIT;

//安全值得设定适用于所有分配到该IV通道下的所有同型辅助通道
typedef struct
{
    unsigned char m_Source_CANID;  //源 CANID以便回复时用
    unsigned char m_IV_index;  	//IV通道号
    unsigned char m_aux_type;		//辅助类型
    float					m_safe_value[8];    //安全值  0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
    // unsigned long long m_beyond_SafeRange[2];  // 用共128个bit。每个bit来记录每个辅助通道的超越安全范围的变化情况。
    // unsigned char m_bUnsafe;
    unsigned char m_safety_alive[8];     //标记该safety是否已被清除失效;0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
    //unsigned char m_Start_Chn_InUnit;			//该辅助类型在其unit中的起始通道号
    //unsigned char m_End_Chn_InUnit;
} ST_AUX_SAFETY;

typedef struct
{
    unsigned char m_Source_CANID;  //源 CANID以便回复时用
    unsigned char m_IV_index;  	//IV通道号
    unsigned char m_aux_type;		//辅助类型
    unsigned char m_High_or_Low; //高端设定还是低端设定// 规定 =0 是低端设定， =1 是高端设定//用来接受从CAN过来的值以便存放
    float					m_safe_value;    //安全值  0=AUX_SAFETY_LOW_END; 1=AUX_SAFETY_HIGH_END
} ST_AUX_HALF_SAFETY;  //这个结构主要用于从CANParse传safety到Aux_DataManager

typedef struct
{
    unsigned char 		m_Source_CANID;  //源 CANID以便回复时用
    unsigned char     m_IV_index;         //为IV通道号
    unsigned char     m_aux_type;		  //辅助类型
    float             m_fLogDeltaRate;      //为PC记录数据增量值比率
    UWord64           m_log_interval;    //为PC记录数据的间隔值
    unsigned char     m_sdl_alive;       //为PC标记该sdl是否已被清除失效
} ST_AUX_SDL;

typedef struct
{
    unsigned char m_Source_CANID;  //源 CANID以便回复时用
    unsigned char m_IV_index;  	//IV通道号
    unsigned char m_aux_type;		//辅助类型
    unsigned char m_aux_chn_index; //辅助通道号
    unsigned char m_data_type; //数据类型，原始数据0， 一阶时间导数 1， 二阶时间导数2
    unsigned char m_single_or_repeat; //只要一次呢还是重复发生。只要一次要确认发送成功，重复发生的不需要确认
    unsigned  char    m_TimeDelta;          //预留    可以设置多个等级10ms 100ms 1000ms等（可以不用浪费这么多bit）
    unsigned  char    m_MetaDelta;        //预留，变量delta 大致如上
} ST_AUX_DATA_REQUEST;

typedef struct
{
    unsigned char  ChnNo;
    unsigned char  Auxtype ;
    unsigned short  AuxChn;
    unsigned char  DestID;
    unsigned char  DataType;
    unsigned char  AckStatus;
    unsigned char  ErrCode;
    float          f_Value;
} ST_AUX_DATAPOINT;
typedef struct
{
    unsigned char m_IV_unitID;
    unsigned char m_AuxChnIdx;
    unsigned char m_AuxType;
    unsigned char m_AuxSensor;
    unsigned char m_IVChnIdx;
    unsigned char m_RptDest;  //预留
    MsTime    TrigTime;
} ST_AUX_DATA_TRIG;

typedef struct
{
    unsigned char  m_ID; //用于保存那些用到了本辅助IV Uunit 的unit ID，对没用到，可以在轮询的时候跳过
    unsigned char  m_Valid; //用来标记MAP缓存的那些IVMCU有效。只有MAP过的单元才有效
} ST_IVMCUID_CFG;

typedef struct
{
    bool            m_bValid;
    //bool		      m_bOut;
    unsigned char   m_BID;
    unsigned char   m_CID;
    Aux_Ctrl_Value  m_Out;
} ST_Aux_oneCtrl;


#if DEBUG_TRACE_RECORD == 1
typedef struct
{
    unsigned short   m_u16RecordType;
    MsTime           m_RecordTime;

    unsigned long    m_u32Parameter1;
    unsigned long    m_u32Parameter2;
    unsigned long    m_u32Parameter3;
    unsigned long    m_u32Parameter4;

    float            m_fParameter1;
    float            m_fParameter2;
} St_DebugPoint;
typedef CIRCULAR_QUEUE(St_DebugPoint,DEBUG_TRACE_RECORD_DEPTH) St_DebugTraceRecord;
#endif
typedef struct
{
    unsigned  char  AUXTYPE ;
    unsigned  char AUX_SUBTYPE;
    unsigned  char Index_for_SameAuxtype;
} ST_ADC_CHANNEL;

typedef struct                   //采样配置结构，参照总部设计
{
    unsigned char m_AdcType;    //ADC类型
    unsigned char m_AdcSPIType; //ADC用的SPI/I2C总线
    unsigned char m_AdcChnNum;//ADC支持的电气通道数
    SSPI_Para     m_SSPCfg;   //按照通道执行SSP配置操作
    My_Func_Ptr  m_ADCSampleFunc; //该通道所使用的采样芯片对应的采样驱动程序
    unsigned char BeUsed;
    ST_ADC_CHANNEL  m_adc_channel [MAX_AUX_CHANNEL_NUM_PER_ADC];  // MAX_AUX_CHANNEL_NUM_PER_ADC每片ADC支持的最多通道数，目前暂定为16
} ST_ADCCFG;

typedef struct               //因为有可能遇到混合类型的板，所以不能用Union
{
    ST_ADCCFG			m_AdcCfg[MAX_ADC_CHIPS];                //采样配置结构
    //ST_DICFG		m_DICfg[MAXDICHANNEL];
    //ST_DOCFG		m_DOCfg[MAXDOCHANNEL];

} ST_SINGLEBOARDCFG;          //单板配置信息结构

typedef struct
{
    unsigned char BoardTYPE;     //板类型
    unsigned char BoardID;		  //板编号
    unsigned char BeUsed;       //是否被使用
    unsigned char m_ChannelCount; //有效通道数量
    unsigned char ChannelADC[16]; //描述每个通道对应得ADC ID号
    ST_SINGLEBOARDCFG          m_SingleBoardCFG;  //单板配置信息
} ST_AUX_BOARDCFG;             //MCU支持的辅助配置信息

//针对于MCU的Aux channel info结构
typedef struct
{
    unsigned long AdcCode;
    unsigned char NewAdcCode;

    float PV;
} st_Aux_Chn_Info;

////////////以下为renew
typedef struct
{
    bool exist;    //是否真实存在  配置存在的
    bool inUsed;     //是否在应用中  大概是跑sch中的
    bool fastChn;    //是否快速通道
    uint8 counter;    //用来间隔不在应用的采样

} st_Aux_SampleSetting;

typedef struct
{
    st_Aux_SampleSetting sampleSetting[17];    //17 算上温度特殊通道
    unsigned char sspChn;    //ssp chn
    unsigned char csIndex;                      //cs口，用于cs数组

    unsigned char ADCid;    //if 99 no used
    //	unsigned char type;       //预留，同时在温度中用于K，J，等判断
    unsigned char type[MAX_CH_IN_AUX_UNIT];       //每个通道是K，还是J 同时在温度中用于K，J，等判断  03.19.2014

    unsigned char ADC_chn;          //当前trigger的通道
    unsigned char stateMachine;    //内部状态机
} st_Aux_Sample;


typedef struct
{
    unsigned char   m_MapDIIdx;
    unsigned char   m_MapDOIdx;
    const St_Port * m_pSt_DIPort;
    const St_Port * m_pSt_DOPort;
} St_DIDOMapMgr;

typedef struct
{
    unsigned char   m_MapDOIdx;
    const St_Port * m_pSt_DOPort;
} St_DOMapMgr;

typedef struct
{
    unsigned char e_data_group_id;
    unsigned char e_chan_id;
} ST_AUX_P2E_IN_UNIT;	// Yifei2

