#pragma once

#define LOGGEST_FAST_STAGE                      (1)//1 ms以下为快脉冲
#define MIN_PULSE_STAGE_MILLI_SECONDS			(0.1f)
#define MIN_PULSE_STAGE_INC_MILLI_SECONDS		(0.01f)
#define MAX_PULSE_STAGE_MILLI_SECONDS			(60.0f*60.0f*1000.0f)
#define MIN_PULSE_LOG_DATA_SECONDS				(1.0f)//暂时留着
#define MAX_SINGLE_PULSE_TOTAL_MILLI_SECONDS	(10000)   // 500ms --> 10000ms single pulse   8.10.2007 //05.21.2012 06.28.2012 modify from 10s to 500ms  )//暂时留着

#define SAVE_2_HEAD                             0         //存到头点缓冲
#define SAVE_2_TAIL                             1         //存到尾点缓冲

typedef struct
{
    float             m_fUS;
    float             m_fMS;
    unsigned long     m_u32TimerSet;
} PULSE_RISE_TIME;
typedef struct
{
    float             m_fElapsedTime;
    bool              m_bResume;
    unsigned char     m_StgIdx;
} PULSE_RESUME_INFO;
typedef  struct
{
    bool              m_bFull;           //是否已填满
    bool              m_bDataLogged;     //是否已上传数据
    MsTime            m_CycleStartTime;  //该cycle开始时的系统时间
    unsigned long     m_CycleIdx;
} PULSE_BUF_INFO;     //用于管理缓冲区用
typedef struct
{
    unsigned char     m_uStageNum;                //台阶号
    unsigned long     m_uI_ADC_Code;              //电流DAC码值
    unsigned long     m_uV_ADC_Code;              //电压DAC码值
    float             m_fCurrent;                 //电流浮点数
    float             m_fVoltage;                 //电压浮点数
    unsigned short    m_uElapsedTimeWithinStage;  //该点在该stage内的理论采样时间 us
    bool              m_bSampleUpdated;           //采样更新标志
    bool              m_bDataConverted;           //采样是否被转换成浮点数
    //bool              m_bDataLogged;              //是否已经上报
    bool			  m_bSumCapacity;             //用于累计电量
    bool              m_bSafetyChecked;            //用于安全检查
    unsigned long     m_dataPoint;
} PULSE_RAW_DATA;     //用于保存每个点的原始数据
typedef struct
{
    //unsigned int      m_uHead_Index;              //该stage的第一个点的数据在数据缓冲里的位置
    //unsigned int      m_uTail_Index;              //该stage的最新/最后一个点的数据在数据缓冲里的位置

    bool              m_bFastStage;               //冗余，也可以从脉冲信息获取， 是快stage还是慢stage
    MsTime            m_StageStartTime;           // 该stage开始时获取的系统时间

    unsigned long     m_TimeInUS;                 //从脉冲周期起始的以 微秒 为单位的时间
    //其他需要添加的信息
} PULSE_STAGE_INFO;  //用于管理每个脉冲stage的数据用

typedef struct
{
    bool              m_bFast;         //是否是快速台阶
    unsigned char     m_Non_V_Limit_Priority;
    bool              m_bVerySlow;     //针对stage宽度高于60秒，因为需要额外重新设定Timer的预分频寄存器
    unsigned long     m_PredictAdcCode;  //预计的ADC码值，用于ADC合理性推测
    unsigned long     m_PredictAdcCode_Max_Head;
    unsigned long     m_PredictAdcCode_Min_Head;
    unsigned long     m_PredictAdcCode_Max_Tail;
    unsigned long     m_PredictAdcCode_Min_Tail;
    unsigned long     m_DacCode;
    unsigned long     m_DacCode_NextStg; //下一个Stage的DAC码值
    unsigned long     m_TimerSet;
// 	unsigned long     m_TimerSet2;
// 	unsigned long     m_TimerSet3;
} St_PulseStageInfo_Running;   //运行时的脉冲台阶结构定义
typedef struct
{
    bool              m_bPulseSample;  //用于标识是否启动普通采样流程:   0-->PULSE没有采样，普通采样开启    1-->PULSE正在采样，普通采样关闭；
    unsigned char     m_StageCount;    //台阶总数
    unsigned char     m_StageIdx;      //正在运行的台阶序号
    //MsTime            m_StartTime;     //pulse的第一个stage启动时的系统时间，以100us为分辨率
    unsigned long     m_CycleCounter;  //pulse总共完成了多少次周期？

    unsigned char     m_SampType;      //0:普通记录，每个pulse stage记录两个点； 1:特殊记录，每个pulse stage记录尾巴一个点；

    St_PulseStageInfo_Running    m_Stages[MAX_PULSE_STAGE_NUM];
} St_PulseInfo_Running;

typedef struct
{
    WORD               m_wChannelNumber;
    BYTE               m_btIfPartialPusle;                  // 是否分批脉冲。	// 0表示完整脉冲  // 1表示不完整脉冲
    DWORD              m_dwPulseCycleID;                    // 脉冲Cycle号。预留，将来可能进行记录
    BYTE               m_btPointCount;                      // 本包内点数。注意点数不是Stage数。有n个Stage的脉冲实际上有2n个点。
    BYTE               m_btFirstPointID;                    // 本包内第一点的点号。第k个Stage的第m个点，其点号为2(k-1)+m-1。举例来说，第一个Stage的第一个点其点号为0；第十个Stage的最后一点其点号为19。这里需要注意以后是否不限制每个Stage两个点，那样的话需要重新设计。
    BYTE               m_btIfHaveCapEng;                    // 本包是否包含电量能量数据。这个可以讨论，是否脉冲数据就不带电量能量，电量能量数据另外以普通数据点上传
    BYTE               m_btbuf_Idex;                        // 用于管理缓冲区的前后切换号码，如0号或1号
    MsTime             m_PulseStartTime;
} St_Pulse_PackInfo;

typedef struct
{
    unsigned long     m_uI_ADC_Code_Head;              //电流DAC码值
    unsigned long     m_uV_ADC_Code_Head;              //电压DAC码值
    unsigned long     m_uI_ADC_Code_Tail;              //电流DAC码值
    unsigned long     m_uV_ADC_Code_Tail;              //电压DAC码值
} HistorySampleStg;
typedef struct
{
    HistorySampleStg  m_Stags[MAX_PULSE_STAGE_NUM];
    bool              bValid;
} PulseHistorySample;
enum
{
    PulseSM2_Dac,
    PulseSM2_Sample1,
    PulseSM2_Sample2Pre,    //在进行第二次采样的时候，提前关闭 普通采样任务，防止干扰
    PulseSM2_Sample2,
    PulseSM2_End,
};
enum
{
    PulseSM_Dac,
    PulseSM_Sample1_Conv,
    PulseSM_Sample1_Read,
    PulseSM_Sample2_Conv,
    PulseSM_Sample2_Read,
    PulseSM_End,
};

enum
{
    PulseSM3_Dac,
    PulseSM3_Sample1_Conv1,
    PulseSM3_Sample1_Read1Conv2,
    PulseSM3_Sample1_Read2Conv3,
    PulseSM3_Sample1_Read3,
    PulseSM3_Sample2_Conv1,
    PulseSM3_Sample2_Read1Conv2,
    PulseSM3_Sample2_Read2Conv3,
    PulseSM3_Sample2_Read3,
    PulseSM3_End,
};

class Pulse
{
private:
    //static unsigned char                   m_StateMachine;       //下位机台阶流转状态机
    static St_PulseStageInfo_Running       *m_StagePtr;          //台阶运行指针，指向正在运行的台阶定义
    static PULSE_RESUME_INFO               m_ResumeInfo;
    static PULSE_RISE_TIME                 m_RiseTime;
    static PulseHistorySample              m_HistorySample[3];
    static PulseHistorySample              *m_pSampleLast1;
    static PulseHistorySample              *m_pSampleLast2;
    static PulseHistorySample              *m_pSampleCurrent;
    static unsigned long                   m_SamleErrorCounter;

public:
    static unsigned char                   m_StateMachine;       //下位机台阶流转状态机
    static St_PulseInfo_Running            m_Info;               //运行态的PULSE定义，   从上位机下载的原始PULSE定义PULSE_DEF翻译而来
    static PULSE_RAW_DATA                  raw_data[2][2][ MAX_PULSE_STAGE_NUM] ; //[缓冲区1/2][头/尾][台阶数] [2][2][];
    //头尾分开放以便于记录一个点(只记录尾点)和记录两个点（首，尾两点）

    static PULSE_STAGE_INFO                stage_info[2][ MAX_PULSE_STAGE_NUM]; //对应于两个缓冲区的，有关脉冲stage的数据索引
    static PULSE_BUF_INFO	                 buf_info[2];
    static unsigned char                   active_index;            //pulse两个缓冲区，正在填充数据的 序号
    //static unsigned char                   stage_count;
    static bool			                       Pulse_is_Running;  //标记是否已开始做脉冲任务
    //static bool                            DoingAdc;
    static float                           fMaxVoltage;
    static float                           fMinVoltage;
    static float                           fMaxVoltage_Curr;      //2014/12/26,per ATL's request
    static float                           fMinVoltage_Curr;      //2014/12/26,per ATL's request
    static float                           fMaxCurrent;
    static float                           fMinCurrent;
    static float                           fMaxPower;
    static float                           fMinPower;

public:
    static void        SetRiseTime(float fRiseTimeInMs);
    static void        MVinit(void);
    static void        PulseInit(unsigned char ChannelIdx);
    static void        Start(unsigned char ChannelIdx);
    static void        End(unsigned char ChannelIdx);
    static void        PulseISR3(void);
    static inline void SaveRawData(unsigned char StgIdx,unsigned long Current,unsigned long Voltage,unsigned char head_or_tail);
    static inline void ReadAdcAfterBusy (unsigned long *Data1,unsigned long *Data2);
    static inline void Dac_SendData(unsigned long value);
};
