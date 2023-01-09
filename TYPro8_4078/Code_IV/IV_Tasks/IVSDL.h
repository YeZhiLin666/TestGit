#pragma once

#define SDL_QUEUE_DEPTH                1024   //1K���
#define SDL_QUEUE_DEPTH_3IN4           SDL_QUEUE_DEPTH*3/4
#define SDL_QUEUE_DEPTH_7IN8           SDL_QUEUE_DEPTH*7/8
#define MULTI_FACTOR                   20     //delta ��20��������Ϊ���������½��أ��������뻬�������˲���
#define ACCURACY                       0.001  //�����豸����Ϊ ǧ��֮һ
#define SDL_HISTORY_DEPTH              200
#define SDL_HISTORY_COMPRESS_DEPTH     SDL_HISTORY_DEPTH*0.6


#define SDL_MAX_PLOG                   (8+1+2)    // IR-7 ACR-8  dVdt-9 CCTime-10 DCTime-11
#define INTERVALTIME_T1S               1   // ���ʱ��1s
#define INTERVALTIME_T5S               5   // ���ʱ��5s
#define INTERVALTIME_T10S              10  // ���ʱ��10s
#define INTERVALTIME_T30S              30  // ���ʱ��30s
#define INTERVALTIME_T50S              50  // ���ʱ��50s

#define IV_SUB_SDL_FACTOR_I            0.002f  // 0.002f  // 0.001f     //delta I = 0.005*I RANGE 
#define IV_SUB_SDL_TIME_NORMAL         0.05f  // 0.05f   //0.1f        //50ms report tick
#define IV_SUB_SDL_TIME_FREQUENTLY     0.01f  // SubMaster 10ms ���ٻ㱨����ֵ 
#define IV_SUB_SDL_FREQUENTLY_COUNT    2      //���ٻ㱨����
#define IV_SUB_SDL_TIME_FREQUENTLY_FOR_DBPT     0.0001f  // SubMaster 100us ���ٻ㱨����ֵ �����DBPT����  //09.06.2018
#define IV_SUB_SDL_FREQUENTLY_COUNT_FOR_DBPT		20		////���ٻ㱨����
#define IV_SUB_SDL_TIME_IDLE           1.0f   //��ʱ1��㱨һ��
#define IV_SUB_SDL_TIME_RAPIDLY_COMMUNICATE     0.005f   // SubMaster 5ms ���ٻ㱨����ֵ��RUDPҲ�ӿ�˫��ȷ�ϰ�

//#if SUPPORT_AUX == 1
#define AUXLOG_PC_DATA                 (0X01 << 0)        //ΪPC��¼����  
#define AUXREPORT_IV_DATA              (0X01 << 1)        //ΪIV��������
#define AUXDATA_PC_AND_IV              (LOG_PC_DATA | REPORT_IV_DATA)     //ΪPC��IV��������
#define LOG_ALL_MVUD                   (0xFF)

#define DEBUG_TEST_IR_MODE         0           //0�����ܱ���ֹ��  1�����ܱ����ã�

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

    unsigned char         bCriticalPoint;    //�Ƿ�Ϊ�ؼ��㣬���ڿ���ѹ����ʱ����ɾ�������ݵ㣩
    unsigned char         bLoged;            //���Point�Ƿ�Log��

    unsigned long         u32Value;
    float                 fValue;            //�������ֵ
    float                 fValue2;           //����ĸ���ֵ�����fValue�ǵ�����fValue2���ǵ�ѹ����֮����֮��

    bool bMasterPointEnQ;  //flag to enq slave data whenever master point is pushed in.
} St_SDL_History_Point;   //ռ��16���ֽڣ�SDL buffer�������ݵ�Ԫ

typedef CIRCULAR_QUEUE(St_SDL_Point_4BytesCycleIdx,SDL_QUEUE_DEPTH)             St_SDL_QUEUE;            //ռ��16���ֽ�
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

    float                 f_Time;               //��λ�����ص�ʱ���� �� Ϊ��λ
    unsigned long long    u64Time;              //��λ������ʱ���Ƚϵ�ʱ���� ��΢�� Ϊ��λ

//    float                 fCapacity;
//    float                 fEnergy;
} St_DeltaValue;
typedef struct  //St_ChannelStatus
{   //���ṹ���浱ǰSTEP��ԭʼ����
    unsigned short        u16ControlType;
    unsigned char         ucCurrentRange;
    unsigned char         ucVoltageRange;

    float                 dXdt;              //dIdt����dVdt

//    bool                  bFirstAdcLogged_I;   //������ĵ�һ�����Ƿ��Ѿ���LOG��?
//    bool                  bFirstAdcLogged_V;   //������ĵ�һ�����Ƿ��Ѿ���LOG��?
    unsigned char         ExpCondition;
//    unsigned char         bNeedSDL;          //��־��step�Ƿ���ҪSDL���ܣ�

    unsigned char         bIdle;

    float                 fR;                //������Զ����ǵ��裬�����е���� ŷķ����
    float                 fBatteryCapacity;  //������Զ����ǵ�أ������е�ص� �������
    float                 fBatteryVoltage;   //������Զ����ǵ�أ������е�ص� ���ѹ��

    float                 Vmax;
    float                 Imax;

    float                 fCurrentAccuracy;   //������Config/Schedule���õĵ�������ֵ
    float                 fVoltageAccuracy;   //������Config/Schedule���õĵ�ѹ����ֵ
    float                 fCapacityAccuracy;  //������Config/Schedule���õ���������ֵ
    float                 fEnergyAccuracy;    //������Config/Schedule���õ���������ֵ
} St_ChannelStatus;


typedef struct
{
    bool							bNeedFrequentlyLog;	 //�Ƿ���Ҫ���ٻ㱨
    unsigned char	 				LogCount;            //��¼Sub Master���ٻ㱨�Ĵ���
} St_FrequentlyLog;


//#if SUPPORT_AUX == 1
typedef struct
{
    unsigned char         MCUId;     //ΪIV��������MCU��
    unsigned char         bReportIvData;     //ΪIV��������
    UWord64               ReportInterval;  //ΪIV��������ʱ����
} St_AUXNeedValue;

typedef struct
{
    unsigned char         bLogPcData;        //ΪPC��¼����
    float                 fLogDeltaValue;    //ΪPC��¼�����������������ȡ�����ֵ����ǧ��֮��
    St_AUXNeedValue       NeedValue[MAXIV4AUXCHANNELNO];         //ΪIV��������
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
	static MsTime 						ReportLog_time;	//	���ڴ��LOGʱ��㣬ͳһIV��AUX��LOG���ϱ�ʱ��
    static St_SDL_QUEUE                 Buffer;                                //SDL buffer
    static unsigned short               Priority;
    static unsigned short               TaskID;
    static unsigned char                Valid;
//    static unsigned long                CurrentCounter[MAXCHANNELNO];
//    static unsigned long                VoltageCounter[MAXCHANNELNO];
    static St_Grp                       m_GrpInfo4SDL[MAXCHANNELNO];
//    static unsigned char                uc_IR_SendCount[MAXCHANNELNO];             // ����IR���ݲ������ϴ�������
    static unsigned char                uc_ACR_SendCount[MAXCHANNELNO];            // ����ACR���ݲ������ϴ�������
//    static UWord32                      uw32_IntervalTCapacity[MAXCHANNELNO];   // �����������¼ʱ��
//    static UWord32                      uw32_IntervalTEnergy[MAXCHANNELNO];     // �����������¼ʱ��
    static UWord32                      uw32_Interval_4_ETable[MAXCHANNELNO];                  //Delay for E_table
//#if SUPPORT_AUX == 1  //����ΪIV��ר��������� dirui 0304
//    static St_AUXSDL_Info               m_AUXInfo[MAXCHANNELNO][MAXAUXTYPE];
//    static ST_AUX_SDL					          m_AuxSDLBank[MAXCHANNELNO][MAXAUXTYPE]; //SDL�������������ͷ֣���IVͨ��֮�����������ͬ
//    static ST_AUX_DATA_REQUEST 			    m_AuxDataRequestBank[MAXCHANNELNO][3*MAX_AUXCH_PER_AUXMCU]; //���ڵǼ���Щ����ͨ�����ݱ�������,*3 ����Ϊ��3������
//    static St_AUXSDL_LastValue		      m_LastAUXValue[MAXCHANNELNO][MAXAUXTYPE][MAX_AUXCH_PER_AUXMCU];
//#endif
    static MsTime                  	    m_IntervalTime_SDL;
//	static unsigned long                m_dwStepPoint[MAXCHANNELNO]; //mits8
//    static float						m_LastBuiltInValue[MAXCHANNELNO][MAX_AUXCH_PER_DATA_GROUP];
    static ST_BuiltIn_DATA   							st_BuiltIn_Aux_Data;
	static St_DeltaValue                DeltaValue[MAXCHANNELNO];              //ÿ��������� Deltaֵ��DCRû��Deltaֵ
private:
//    static unsigned char                m_bPairIV;                             //��������ѹ�ɶ�LOG
//    static unsigned char                ucLowSpeedFactor;                      //���ڵ���ģʽ��1   2   4
//    static unsigned char                ucLowSpeedFactor_New;                  //���ڵ���ģʽ��1   2   4
//    static unsigned char                bHaveLoggedOnePoint;                   //�Ƿ�Log��һ��ͨ����
//    static unsigned short               CurrentChannelIdx;                     //SDL���÷�ʱ���ƣ�ÿ��SDL����ֻ����һ��ͨ����һ��POINT�����Դ���SDLĿǰ���ڴ����ͨ���ű�ǡ�
    static MsTime                       LastLogTime[MAXCHANNELNO];             //��һ�β�����ʱ��㣻
    static St_FrequentlyLog				SubMasterFrequentlyLog;
//    static float                        LastLoggedCCapacity[MAXCHANNELNO];     //�ϴμ�¼�� ���������
//    static float                        LastLoggedDCapacity[MAXCHANNELNO];     //�ϴμ�¼�� �ŵ�������
//    static float                        LastLoggedCEnergy[MAXCHANNELNO];       //�ϴμ�¼�� ���������
//    static float                        LastLoggedDEnergy[MAXCHANNELNO];       //�ϴμ�¼�� �ŵ�������
//    static St_SDL_History_Point         LastLoggedCurrent[MAXCHANNELNO];       //�ϴμ�¼�� ������
//    static St_SDL_History_Point         LastLoggedVoltage[MAXCHANNELNO];       //�ϴμ�¼�� ��ѹ��
//    static St_SDL_History_Point         PreviousCurrent[MAXCHANNELNO];         //�ϴβ������� ������
//    static St_SDL_History_Point         PreviousVoltage[MAXCHANNELNO];         //�ϴβ������� ��ѹ��
//    static St_SDL_History_Point         FirstLoggedCurrent[MAXCHANNELNO];      //��һ����Ϊƫ�Ƶ��±�LOG�ļ�¼�㣬��������dIdt��
//    static St_SDL_History_Point         FirstLoggedVoltage[MAXCHANNELNO];      //��һ����Ϊƫ�Ƶ��±�LOG�ļ�¼�㣬��������dVdt��
//     static St_DeltaValue                DeltaValue[MAXCHANNELNO];              //ÿ��������� Deltaֵ��DCRû��Deltaֵ
//    static St_SDL_HISTORY_QUEUE         HistoryCurrentData[MAXCHANNELNO];      //Ϊÿ��ͨ������һ��������ʷ���ݶ���
//    static St_SDL_HISTORY_QUEUE         HistoryVoltageData[MAXCHANNELNO];      //Ϊÿ��ͨ������һ����ѹ��ʷ���ݶ���
//    static St_SDL_History_Point         LastCurrentAdcPoint[MAXCHANNELNO];     //��һ��ADC�����ĵ�����
//    static St_SDL_History_Point         LastVoltageAdcPoint[MAXCHANNELNO];        //��һ��ADC�����ĵ�ѹ��
    static St_ChannelStatus             ChannelStatus[MAXCHANNELNO];           //��¼ÿ��ͨ���ĵ�ǰ����״̬���Ǽ���Deltaֵ��Դͷ
//    static unsigned char                uc_DischargeCFlag[MAXCHANNELNO];       // �ŵ�/�������������¼�л���־
//    static unsigned char                uc_DischargeEFlag[MAXCHANNELNO];       // �ŵ�/�������������¼�л���־
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

    static inline void SubMasterSdlLogTimesFrequently2Normal(unsigned short ChannelIdx);//��SubMaster�ӿ��ٴ��ת��Ϊ�������
    static void SubMasterSdlLogTimesNormal2Frequently(unsigned short ChannelIdx);//��SubMaster���������ת��Ϊ���ٴ��
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

