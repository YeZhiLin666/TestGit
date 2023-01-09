#pragma once

#define SDL_QUEUE_DEPTH                1024   //1K深度
#define SDL_QUEUE_DEPTH_3IN4           SDL_QUEUE_DEPTH*3/4
#define SDL_QUEUE_DEPTH_7IN8           SDL_QUEUE_DEPTH*7/8
#define MULTI_FACTOR                   20     //delta 的20倍，被认为是上升、下降沿，将不进入滑动窗口滤波器
#define ACCURACY                       0.001  //设置设备精度为 千分之一
#define SDL_HISTORY_DEPTH              200
#define SDL_HISTORY_COMPRESS_DEPTH     SDL_HISTORY_DEPTH*0.6


#define SDL_MAX_PLOG                   (8+1+2)    // IR-7 ACR-8  dVdt-9 CCTime-10 DCTime-11
#define INTERVALTIME_T1S               1   // 间隔时间1s
#define INTERVALTIME_T5S               5   // 间隔时间5s
#define INTERVALTIME_T10S              10  // 间隔时间10s
#define INTERVALTIME_T30S              30  // 间隔时间30s
#define INTERVALTIME_T50S              50  // 间隔时间50s

#define IV_SUB_SDL_FACTOR_I            0.002f  // 0.002f  // 0.001f     //delta I = 0.005*I RANGE 
#define IV_SUB_SDL_TIME_NORMAL         0.05f  // 0.05f   //0.1f        //50ms report tick
#define IV_SUB_SDL_TIME_FREQUENTLY     0.01f  // SubMaster 10ms 快速汇报采样值 
#define IV_SUB_SDL_FREQUENTLY_COUNT    2      //快速汇报次数
#define IV_SUB_SDL_TIME_FREQUENTLY_FOR_DBPT     0.0001f  // SubMaster 100us 快速汇报采样值 仅针对DBPT测试  //09.06.2018
#define IV_SUB_SDL_FREQUENTLY_COUNT_FOR_DBPT		20		////快速汇报次数
#define IV_SUB_SDL_TIME_IDLE           1.0f   //闲时1秒汇报一次
#define IV_SUB_SDL_TIME_RAPIDLY_COMMUNICATE     0.005f   // SubMaster 5ms 快速汇报采样值，RUDP也加快双方确认包

//#if SUPPORT_AUX == 1
#define AUXLOG_PC_DATA                 (0X01 << 0)        //为PC记录数据  
#define AUXREPORT_IV_DATA              (0X01 << 1)        //为IV报告数据
#define AUXDATA_PC_AND_IV              (LOG_PC_DATA | REPORT_IV_DATA)     //为PC与IV报告数据
#define LOG_ALL_MVUD                   (0xFF)

#define DEBUG_TEST_IR_MODE         0           //0：功能被禁止；  1：功能被启用；

enum TE_AUX_LOGSWITCH
{
    LOGSWITCH_OFF = 0,
    LOGSWITCH_ON,
};
//#endif
enum eExpCondition
{
    eExpCondition_Battery,
    eExpCondition_Resistance,
};

enum SDL_Type
{
    SDL_CURRENT,
    SDL_VOLTAGE,
    SDL_FLOAT_CURRENT,
};

enum FACTOR_TYPE
{
    FACTOR_I,
    FACTOR_V,
    FACTOR_Cap,
    FACTOR_Energy,
};
typedef struct  //St_SDL_History_Point
{
    MsTime                Time;

    unsigned char         bCriticalPoint;    //是否为关键点，（在考虑压缩的时候不能删除的数据点）
    unsigned char         bLoged;            //这个Point是否被Log了

    unsigned long         u32Value;
    float                 fValue;            //数据项浮点值
    float                 fValue2;           //额外的浮点值，如果fValue是电流，fValue2就是电压；反之，则反之；

    bool bMasterPointEnQ;  //flag to enq slave data whenever master point is pushed in.
} St_SDL_History_Point;   //占用16个字节，SDL buffer基本数据单元

typedef CIRCULAR_QUEUE(St_SDL_Point_4BytesCycleIdx,SDL_QUEUE_DEPTH)             St_SDL_QUEUE;            //占用16个字节
typedef CIRCULAR_QUEUE(St_SDL_History_Point,SDL_HISTORY_DEPTH)   St_SDL_HISTORY_QUEUE;

typedef struct  //St_DeltaValue
{
//    unsigned long         u32Current;
//    float                 fCurrent;
//    float                 f_dIdt;
//    float                 f_dIdt2;

//    unsigned long         u32Voltage;
//    float                 f_dVdt;
//    float                 f_dVdt2;

    float                 f_Time;               //上位机下载的时间以 秒 为单位
    unsigned long long    u64Time;              //下位机在做时间差比较的时候，以 百微秒 为单位

//    float                 fCapacity;
//    float                 fEnergy;
} St_DeltaValue;
typedef struct  //St_ChannelStatus
{   //本结构保存当前STEP的原始参数
    unsigned short        u16ControlType;
    unsigned char         ucCurrentRange;
    unsigned char         ucVoltageRange;

    float                 dXdt;              //dIdt或者dVdt

//    bool                  bFirstAdcLogged_I;   //本步骤的第一个点是否已经被LOG了?
//    bool                  bFirstAdcLogged_V;   //本步骤的第一个点是否已经被LOG了?
    unsigned char         ExpCondition;
//    unsigned char         bNeedSDL;          //标志本step是否需要SDL功能；

    unsigned char         bIdle;

    float                 fR;                //如果测试对象是电阻，这里有电阻的 欧姆数；
    float                 fBatteryCapacity;  //如果测试对象是电池，这里有电池的 额定容量；
    float                 fBatteryVoltage;   //如果测试对象是电池，这里有电池的 额定电压；

    float                 Vmax;
    float                 Imax;

    float                 fCurrentAccuracy;   //来自于Config/Schedule配置的电流精度值
    float                 fVoltageAccuracy;   //来自于Config/Schedule配置的电压精度值
    float                 fCapacityAccuracy;  //来自于Config/Schedule配置的容量精度值
    float                 fEnergyAccuracy;    //来自于Config/Schedule配置的能量精度值
} St_ChannelStatus;


typedef struct
{
    bool							bNeedFrequentlyLog;	 //是否需要快速汇报
    unsigned char	 				LogCount;            //记录Sub Master快速汇报的次数
} St_FrequentlyLog;


//#if SUPPORT_AUX == 1
typedef struct
{
    unsigned char         MCUId;     //为IV报告数据MCU号
    unsigned char         bReportIvData;     //为IV报告数据
    UWord64               ReportInterval;  //为IV报告数据时间间隔
} St_AUXNeedValue;

typedef struct
{
    unsigned char         bLogPcData;        //为PC记录数据
    float                 fLogDeltaValue;    //为PC记录数据增量方便计算先取出最大值乘以千分之几
    St_AUXNeedValue       NeedValue[MAXIV4AUXCHANNELNO];         //为IV报告数据
} St_AUXSDL_Info;

typedef struct
{
    float                 fValue;
    MsTime                LogTime;        //???LOG?PC????

    MsTime                ReportTime;     //???REPORT?IV????

    MsTime                DisplayLogTime;       //??????????PC????
    MsTime                IdlLogTime;          //??????????PC????
} St_AUXSDL_LastValue;
//#endif

typedef struct
{
    bool    bLogAllMVUD;
    unsigned char   LogMVUD_MetaCode;
} ST_LOG_MVUD;
class IVSDL
{
public:
    static bool                         IsChnNeedFilter[MAXCHANNELNO];
	static MsTime 						ReportLog_time;	//	用于存放LOG时间点，统一IV与AUX的LOG点上报时间
    static St_SDL_QUEUE                 Buffer;                                //SDL buffer
    static unsigned short               Priority;
    static unsigned short               TaskID;
    static unsigned char                Valid;
//    static unsigned long                CurrentCounter[MAXCHANNELNO];
//    static unsigned long                VoltageCounter[MAXCHANNELNO];
    static St_Grp                       m_GrpInfo4SDL[MAXCHANNELNO];
//    static unsigned char                uc_IR_SendCount[MAXCHANNELNO];             // 内阻IR数据补救性上传计数器
    static unsigned char                uc_ACR_SendCount[MAXCHANNELNO];            // 内阻ACR数据补救性上传计数器
//    static UWord32                      uw32_IntervalTCapacity[MAXCHANNELNO];   // 电容量间隔记录时间
//    static UWord32                      uw32_IntervalTEnergy[MAXCHANNELNO];     // 电容量间隔记录时间
    static UWord32                      uw32_Interval_4_ETable[MAXCHANNELNO];                  //Delay for E_table
//#if SUPPORT_AUX == 1  //以下为IV的专属辅助添加 dirui 0304
//    static St_AUXSDL_Info               m_AUXInfo[MAXCHANNELNO][MAXAUXTYPE];
//    static ST_AUX_SDL					          m_AuxSDLBank[MAXCHANNELNO][MAXAUXTYPE]; //SDL参数按辅助类型分，但IV通道之间可以有所不同
//    static ST_AUX_DATA_REQUEST 			    m_AuxDataRequestBank[MAXCHANNELNO][3*MAX_AUXCH_PER_AUXMCU]; //用于登记那些辅助通道数据被请求了,*3 是因为有3种数据
//    static St_AUXSDL_LastValue		      m_LastAUXValue[MAXCHANNELNO][MAXAUXTYPE][MAX_AUXCH_PER_AUXMCU];
//#endif
    static MsTime                  	    m_IntervalTime_SDL;
//	static unsigned long                m_dwStepPoint[MAXCHANNELNO]; //mits8
//    static float						m_LastBuiltInValue[MAXCHANNELNO][MAX_AUXCH_PER_DATA_GROUP];
    static ST_BuiltIn_DATA   							st_BuiltIn_Aux_Data;
	static St_DeltaValue                DeltaValue[MAXCHANNELNO];              //每个数据项的 Delta值，DCR没有Delta值
private:
//    static unsigned char                m_bPairIV;                             //电流、电压成对LOG
//    static unsigned char                ucLowSpeedFactor;                      //处于低速模式：1   2   4
//    static unsigned char                ucLowSpeedFactor_New;                  //处于低速模式：1   2   4
//    static unsigned char                bHaveLoggedOnePoint;                   //是否Log了一个通道；
//    static unsigned short               CurrentChannelIdx;                     //SDL采用分时机制，每次SDL周期只处理一个通道的一个POINT，所以存在SDL目前正在处理的通道号标记。
    static MsTime                       LastLogTime[MAXCHANNELNO];             //上一次采样的时间点；
    static St_FrequentlyLog				SubMasterFrequentlyLog;
//    static float                        LastLoggedCCapacity[MAXCHANNELNO];     //上次记录的 充电容量；
//    static float                        LastLoggedDCapacity[MAXCHANNELNO];     //上次记录的 放电容量；
//    static float                        LastLoggedCEnergy[MAXCHANNELNO];       //上次记录的 充电能量；
//    static float                        LastLoggedDEnergy[MAXCHANNELNO];       //上次记录的 放电能量；
//    static St_SDL_History_Point         LastLoggedCurrent[MAXCHANNELNO];       //上次记录的 电流；
//    static St_SDL_History_Point         LastLoggedVoltage[MAXCHANNELNO];       //上次记录的 电压；
//    static St_SDL_History_Point         PreviousCurrent[MAXCHANNELNO];         //上次参与计算的 电流；
//    static St_SDL_History_Point         PreviousVoltage[MAXCHANNELNO];         //上次参与计算的 电压；
//    static St_SDL_History_Point         FirstLoggedCurrent[MAXCHANNELNO];      //第一个因为偏移导致被LOG的记录点，用于修正dIdt；
//    static St_SDL_History_Point         FirstLoggedVoltage[MAXCHANNELNO];      //第一个因为偏移导致被LOG的记录点，用于修正dVdt；
//     static St_DeltaValue                DeltaValue[MAXCHANNELNO];              //每个数据项的 Delta值，DCR没有Delta值
//    static St_SDL_HISTORY_QUEUE         HistoryCurrentData[MAXCHANNELNO];      //为每个通道建立一个电流历史数据队列
//    static St_SDL_HISTORY_QUEUE         HistoryVoltageData[MAXCHANNELNO];      //为每个通道建立一个电压历史数据队列
//    static St_SDL_History_Point         LastCurrentAdcPoint[MAXCHANNELNO];     //上一次ADC采样的电流点
//    static St_SDL_History_Point         LastVoltageAdcPoint[MAXCHANNELNO];        //上一次ADC采样的电压点
    static St_ChannelStatus             ChannelStatus[MAXCHANNELNO];           //记录每个通道的当前控制状态，是计算Delta值的源头
//    static unsigned char                uc_DischargeCFlag[MAXCHANNELNO];       // 放电/充电电容量间隔记录切换标志
//    static unsigned char                uc_DischargeEFlag[MAXCHANNELNO];       // 放电/充电电容量间隔记录切换标志
    static ST_LOG_MVUD                  bLogMVUD[MAXCHANNELNO];             // Log all MV_UD matevarible when log first data point after Start or Resume.
//  static ST_BuiltIn_DATA   							st_BuiltIn_Aux_Data;
public:
    static void Init_WithoutBuffer(void);
    static void Init(void);
//	static void DoWork(void);
// 	static unsigned char NeedSDL(unsigned short ChannelIdx);

    static void ScheduleStartedRoutine(unsigned short ChannelIdx);
    static void ScheduleEndedRoutine(unsigned short ChannelIdx);
    static void SubmasterStartRoutine(unsigned short ChannelIdx);
    static void SubmasterStopRoutine(unsigned short ChannelIdx);
    static void StepStartedRoutine(unsigned short ChannelIdx, bool IsClearHistoryData);
    static void LimitArrivedRoutine(unsigned short ChannelIdx);

    static void SetLogMVUDFlag(unsigned short ChannelIdx, unsigned char MVUD_MetaCode);
    static void ClearLogMVUDFlag(unsigned short ChannelIdx);
    static unsigned char IsLogMVUD(unsigned short ChannelIdx);
    static unsigned char IsLogAllMVUD(unsigned short ChannelIdx);
    static void TriggerLog(unsigned short ChannelIdx, unsigned char uc_LogType, unsigned char uc_bCriticalPoint);
    static void IVLogBaseInfo(unsigned short ChannelIdx, UWord16 u16ControlType, St_SDL_Point_4BytesCycleIdx *MyPoint);
    static void IVLogMetaVar(unsigned short ChannelIdx, unsigned char DataLogFormat, unsigned char uc_LogType,unsigned char uc_bCriticalPoint,\
                             BYTE MetaCode1, BYTE MetaCode2 = 0, BYTE MetaCode3 = 0, St_SDL_Point_4BytesCycleIdx *MyPoint = NULL);
    static void LogDataHead(unsigned short ChannelIdx, unsigned char uc_LogType,unsigned char uc_bCriticalPoint, St_SDL_Point_4BytesCycleIdx * PointData);
    static void IVLogAllMVUD(unsigned short ChannelIdx, unsigned char uc_LogType,unsigned char uc_bCriticalPoint, St_SDL_Point_4BytesCycleIdx *MyPoint);
    static void IVLogTCCountForAccess(unsigned short ChannelIdx);
//     static void PushCurrent2HistoryData(unsigned short ChannelIdx,float fValue,unsigned long u32Value);
//     static void PushCurrent2HistoryData_FloatOnly(unsigned short ChannelIdx,float fValue,unsigned long u32Value);
//     static void PushVoltage2HistoryData(unsigned short ChannelIdx,float fValue,unsigned long u32Value);

    static void GetPoint(unsigned short ChannelIdx,unsigned char DataFor,unsigned char ValueType,unsigned long DataPoint, unsigned long DataFlag, St_Data_Pairs * MyPoint);

    static inline void SubMasterSdlLogTimesFrequently2Normal(unsigned short ChannelIdx);//将SubMaster从快速打点转换为正常打点
    static void SubMasterSdlLogTimesNormal2Frequently(unsigned short ChannelIdx);//将SubMaster从正常打点转换为快速打点
    /////////////////////////////add for built_in Aux   dirui 2017/12/07
    static void Check_BuiltInAuxForLog(unsigned short ChannelIdx,unsigned char uc_LogType);
    static void TriggerLog_BuiltInAux(unsigned char uc_IVUnitBufIdx, unsigned char uc_IVchn, unsigned char uc_AuxChn,unsigned char uc_LogType);
    static void TrigLogBuiltInPoint( ST_BUILTIN_AUX_DATA_TRIG * TrigInfo, St_SDL_Point_Pack * MyPoint);
    static void SetFactor_Time(unsigned short ChannelIdx,float Factor);
    static void SetFactor(unsigned short ChannelIdx, float Factor, int FactorType);
//     static void SetFactor_I(unsigned short ChannelIdx,float Factor);
//     static void SetFactor_V(unsigned short ChannelIdx,float Factor);
//     static void SetFactor_Capacity(unsigned short ChannelIdx,float Factor);
//     static void SetFactor_Energy(unsigned short ChannelIdx,float Factor);
//    static void IVSdlPushPoint(unsigned short ChannelIdx, unsigned char ucDataType, unsigned char ucValueType, unsigned char bCriticalPoint);
    static void Check_Submaster_Report(unsigned short ChannelIdx);
private:
//    static void Check_Normal(unsigned short ChannelIdx);
    //static void Check_Master(unsigned short ChannelIdx);

    static void SDL_SetParameters(unsigned short ChannelIdx);

// 	static inline  unsigned char Check_Current(unsigned char bOK,St_SDL_History_Point HistoryPoint);
// 	static inline  unsigned char Check_Voltage(unsigned char bOK,St_SDL_History_Point HistoryPoint);
// 	static inline  void Check_CCapacity();
// 	static inline  void Check_DCapacity();
// 	static inline  void Check_CEnergy();
// 	static inline  void Check_DEnergy();
//     static inline  void CheckCapEner(unsigned short ChannelIdx);

//    static void SDL_IV(unsigned short ChannelIdx,St_SDL_History_Point Point, unsigned char type);
//    static void fSDL_I(unsigned short ChannelIdx,St_SDL_History_Point CurrentPoint);
    //    static void SDL_V(unsigned short ChannelIdx,St_SDL_History_Point CurrentPoint);

// 	static void SetFactor_Time(unsigned short ChannelIdx,float Factor);
// 	static void SetFactor_I(unsigned short ChannelIdx,float Factor);
// 	static void SetFactor_V(unsigned short ChannelIdx,float Factor);
// 	static void SetFactor_Capacity(unsigned short ChannelIdx,float Factor);
// 	static void SetFactor_Energy(unsigned short ChannelIdx,float Factor);
    static void SetCapacity(unsigned short ChannelIdx,float Value);
    static void SetNVoltage(unsigned short ChannelIdx,float Value);
//    static inline unsigned long long GetTimeTicks(unsigned long FromInsecond,unsigned short FromUs100,unsigned long ToInsecond,unsigned short ToUs100);

    static inline void PushPoint(St_SDL_Point_4BytesCycleIdx MyPoint);
//    static void AdjustSdlSpeed(void);
//    static void AdjustSdlFactor(void);

    static inline float GetFullValue(float HighValue,float LowValue);
    static void Init_SubMasterFrequentlyLog();
};

