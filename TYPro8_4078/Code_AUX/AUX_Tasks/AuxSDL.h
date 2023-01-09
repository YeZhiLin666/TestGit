#pragma once
#define SDL_TASK_CHANNEL_COUNT              4 // 2       //每次SDL任务，处理4个通道

#define DISPLAY_LOGDATA_INTERVAL            20000      //显示数据间隔记录时间  10秒
#define IDL_LOGDATA_INTERVAL                40000      //闲时数据记录时间   12秒

#define SDL_LOGDATA_T_INTERVAL              (IV_DEFAULT_SDL_TIME * 10000) // 50000;  // SDL默认：T 5秒，2VD 100毫秒记录一点给PC
#define SDL_LOGDATA_H_INTERVAL              (IV_DEFAULT_SDL_TIME * 10000) // 50000;  // SDL默认：H 5秒，2VD 100毫秒记录一点给PC, 湿度
#define SDL_LOGDATA_2VD_INTERVAL            (IV_DEFAULT_SDL_TIME * 10000) // 1000;   // SDL默认：T 5秒，2VD 100毫秒记录一点给PC
#define SDL_LOGDATA_EC_INTERVAL             (IV_DEFAULT_SDL_TIME * 10000)
#define SDL_LOGDATA_PRESSURE_INTERVAL       (IV_DEFAULT_SDL_TIME * 10000)
#define SDL_LOGDATA_T_DELTA                 AUX_DEFAULT_SDL_FACTOR_T       //温度SDL参数默认值为1摄氏度变化量     // SDL默认：变化量值T 1度，2VD 0.5V 记录一点给PC
#define SDL_LOGDATA_H_DELTA                 AUX_DEFAULT_SDL_FACTOR_H       //湿度SDL参数默认值为1%浓度变化量     // SDL默认：变化量值T 1度，2VD 0.5V 记录一点给PC
#define SDL_LOGDATA_2VD_DELTA               AUX_DEFAULT_SDL_FACTOR_2ndV   //电压SDL参数默认值为千分之一;  // SDL默认：变化量值T 1度，2VD 0.5V 记录一点给PC
#define SDL_LOGDATA_EC_DELTA                AUX_DEFAULT_SDL_FACTOR_EC_Current   //外部充电电流 SDL参数默认值为千分之一
#define SDL_LOGDATA_PRESSURE_DELTA          AUX_DEFAULT_SDL_FACTOR_PRESSURE    //
//#define MAX_AUX_TYPE                        16
//#define MAX_AUX_TYPE_CHANNEL                16
#define MAX_IV_MCU_COUNT                    64

// 触发记录方式
#define STEP_LOG_IV                         0      //Step Log模式触发记录数据
#define SDL_LOG_DELTA                       1      //SDL Log模式触发记录数据


// 发送数据方式按位域作判断
#define LOG_PC_DATA                        (0X01 << 0)        //为PC记录数据  
#define REPORT_IV_DATA                     (0X01 << 1)        //为IV报告数据
#define DATA_PC_AND_IV                     (LOG_PC_DATA | REPORT_IV_DATA)     //为PC与IV报告数据


enum TE_AUX_LOGSWITCH
{
    LOGSWITCH_OFF = 0,
    LOGSWITCH_ON,
};

enum SDL_LOG_DELTATYPE
{
    LOG_DELTA_NONE,
    LOG_DELTA_TIME,
    LOG_DELTA_META,
    LOG_DELTA_TIME_DEFAULT,
    LOG_DELTA_META_DEFAULT,
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

typedef struct
{
    unsigned char     uc_PCFlag;       //为PC记录数据时间间隔或者增量值标志
    unsigned char     uc_IvFlag;       //为Iv记录数据时间间隔标志
    float             fLogDeltaRate;  //为PC记录数据增量值比率
    float             LogInterval;     //为PC记录数据时间间隔
    float             ReportInterval;  //为IV报告数据时间间隔
} St_SDL_DeltaValue;

typedef struct
{
    float             fValue;
    MsTime            LogTime;        //上一次LOG到PC的时间点

    MsTime            ReportTime;     //上一次REPORT到IV的时间点

    MsTime            DisplayLogTime;       //上一次显示数据记录到PC的时间点
    MsTime            IdlLogTime;          //上一次闲时数据记录到PC的时间点
} St_SDL_LastValue;

typedef struct
{
    unsigned char     MCUId;     //为IV报告数据MCU号
    unsigned char     bReportIvData;      //为IV报告数据
    unsigned long int ReportDataTypebit;  //为IV报告数据类型 原始数据，一阶时间导数，二阶时间导数
    UWord64           ReportInterval;     //为IV报告数据时间间隔
    MsTime            ReportTime_DXDT;    //为IV报告一阶时间导数时间间隔
} St_NeedValue;

typedef struct
{
    unsigned char     bLogPcData;        //为PC记录数据
    float             fLogDeltaValue;    //为PC记录数据增量方便计算先取出最大值乘以千分之几
    St_NeedValue      NeedValue[MAX_CH_IN_AUX_UNIT];         //为IV报告数据
} St_SDL_Info;

class AuxSDL
{
public:
    static unsigned short               Priority;
    static unsigned short               TaskID;

private:
    static unsigned char                m_ScanStartIVUnitBufIdx;    //SDL任务记录首次开始扫描IVUnit号。
    static unsigned char                m_ScanStartIVIdx[MAX_IVMCU_PER_SYS];    //SDL任务记录首次开始扫描IV号。
    static unsigned char                m_ScanStartTypeIdx[MAX_IVMCU_PER_SYS];    //SDL任务记录首次开始扫描辅助类型号。
    static unsigned char                m_CurrentIVUnitBufIdx;
    static unsigned char                m_CurrentIVIdx[MAX_IVMCU_PER_SYS];    //SDL任务每次处理所有的通道太耗时，处理一个通道周期又太长了，
    static unsigned char                m_CurrentTypeIdx[MAX_IVMCU_PER_SYS];    //SDL任务每次处理所有的通道太耗时，处理一个通道周期又太长了，
    static unsigned char                m_CurrentAuxChIdx[MAX_IVMCU_PER_SYS];   //这里使用这三个current变量的目的是，使得每次处理N个通道，N可配置为4,8,16，以后调试决定
    //static St_SDL_DeltaValue            m_DeltaValue[MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE][MAX_AUXCH_PER_BOARD]];
    static St_SDL_LastValue             m_LastValue[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_CH_IN_AUX_UNIT];
    static St_SDL_Info                  m_Info[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU]; //MAX_BOARD_PER_AUXMCU改为MAX_AUX_TYPE 20160714 zyx

public:
    static void Init(void);
    static void DoWork(void);
    static void TriggerLog_Net(unsigned char uc_IVUnitBufIdx,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char uc_LogType);
    static void SDLSetParameterRoutine(unsigned char uc_IVUnitID,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,St_SDL_DeltaValue * fSdlPara);
    static void SDLEndedRoutine(unsigned char uc_IVUnitBufIdx,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
    static void SwitchLog(unsigned char uc_IVUnitID,unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char Send_Logtype,unsigned char bLogOn);
    //static void RequireData(unsigned char uc_IVUnitBufIdx, unsigned char uc_AuxType, unsigned char uc_AuxChn,unsigned char IvMCUIdx,float Factor);
    static void RequireData(ST_AUX_DATA_REQUEST st_data_request);

private:
    static void SetFactor_Time (unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn, unsigned char Send_Logtype,UWord64 Factor);
    static void SetFactor_Value(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn,float Factor);
    static void Check_LogValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
    static void Check_ReportValue(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);

    inline static unsigned long long GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100);
    inline static float GetFullValue(float HighValue,float LowValue);
    static unsigned char MaxValueCheck(unsigned char uc_IVUnit, unsigned char uc_IVchn, unsigned char uc_AuxType, unsigned char uc_AuxChn);
// 		static void  SendDisplayAuxData();
};

