///////////////////////////////////////////////////////////////////////////////////
//File Name: ACR.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						ACR操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#ifndef _ACR_H_
#define _ACR_H_


#define MAX_ACR_RATE                    (100)   // ACR固定频率1KHZ,1毫秒
#define MAX_ACR_T1                      (1)   // 定时次数

#define ACR_MAXPULSE 1
#define PERCRATE     0.05                    //ACR保护测试脉冲的幅度是容量的多少
#define MAXSPECIALACRSTAGETIME  5            //ACR保护测试时间暂时设定为500us
#define  Fpclk       60000000                   //设置的外设时钟为60MHz
#define ACR_INTERVAL100us (Fpclk/10000 - 1)     // 100微秒

#define MAX_ACR_CH_BUFFER               (4*MAXCHANNELNO)

#define ACR_MAXWAIT_TIME                (10000)   //ACR等待操作硬件时间1秒     //1秒
#define ACR_MAXREMAIN_TIME              (30)  //ACR最后剩余时间提前采样码字，避免较大偏差。 30*1毫秒
#define ACR_MAXSCH_TIME                 (3*2) // 12  //ACR运行SCH最长时间6秒   // 测试验证机制时加大12秒
#define ACR_MAXSCH_ENDTIME              (MAXCHANNELNO*4*ACR_MAXSCH_TIME*10000)   //ACR无法获得资料最大时间     //N*4*6秒



// #define ACR_ACIM   1
// #define ACR_ACIM_COM   1


#define ACR_WIRTE_ADD             (32)     // ACREN   ACR2  ACR1 ACR0

#define ACR_EN   0X0010
#define ACR_A2   0X0004
#define ACR_A1   0X0002
#define ACR_A0   0X0001



typedef struct
{
    uint8     uc_UseFlag;
    uint8     uc_ControleType;
} ST_PULSELOCK;

typedef struct
{
    uint8            uc_Ch;
    ST_PULSELOCK     st_Lock;
} ST_SINGLEGLOCK;

typedef struct
{
    uint8     uc_Ch;
    uint8     uc_ControleType;
} ST_PULSECH;

typedef   CIRCULAR_QUEUE(ST_PULSECH, MAX_ACR_CH_BUFFER)    PULSELOCK_CH;


#define ACR_ACREN   1
#define ACR_ADDACR0   1
#define ACR_ADDACR1   1
#define ACR_ADDACR2   1



enum
{
    ACR_H_DAC = 0,
    ACR_L_DAC = 1,
};

enum
{
    ACR_POSITIVE = 0,
    ACR_NEGATIVE = 1,
    ACR_RESULT = 2,
    ACR_R_COUNT,
};

enum
{
    ACR_NONE=0,
    ACR_START,
    ACR_HARDWARE_OPERATION,
    ACR_RUNNING,
    ACR_CALCULATE,
    ACR_END,

    //Timer1
    ACR_TIMER_NONE=0x10,
    ACR_TIMER_HIGH,
    ACR_TIMER_LOW,
    ACR_TIMER_NEGATIVE,
    ACR_TIMER_END,
};


enum
{
    ACR_OFF = 0X00,
    ACR_ON  = 0X01,
};

class ACR
{
private:
    static unsigned char   ACR_NegativeFlag;
    //static unsigned char 	ACR_Status[MAXCHANNELNO];
    static MsTime ACR_EndTime[MAXCHANNELNO];
    static MsTime ACR_StartTime[MAXCHANNELNO];	//0819
    //static MsTime ACR_TimeInterval[MAXCHANNELNO];
    //static unsigned long  ACR_CycleNum[MAXCHANNELNO];
    //static unsigned long  ACR_CycleCount[MAXCHANNELNO];
    static unsigned short  ACR_T1_Count[MAXCHANNELNO];
    static unsigned char   ACR_H_L[MAXCHANNELNO];
    static unsigned long   ACR_Positive_Dac[MAXCHANNELNO][2];  // 正向DAC
    static unsigned long   ACR_Negative_Dac[MAXCHANNELNO][2];  // 负向DAC
    static float           ACR_I_Offset[MAXCHANNELNO];  // 电流偏移
    static float           ACR_R_V[ACR_R_COUNT];                 // 电阻值
    static PULSELOCK_CH    Lock_Ch_Copy;
public:
    static unsigned char  ACR_Status[MAXCHANNELNO];
    static unsigned long  ACR_CycleNum[MAXCHANNELNO];
    static unsigned long  ACR_CycleCount[MAXCHANNELNO];
    static ST_SINGLEGLOCK  st_Single_Lock;      // 单个执行体
    static ST_PULSELOCK   st_Pulse_Lock[MAXCHANNELNO];  // 实体队列
    static unsigned char  uc_PLock_Count;
    static PULSELOCK_CH   Pulse_Lock_Ch;    // 管理队列
    static unsigned char  uc_Pulse_Clear;   // 脉冲换步脉冲
    static float SaveACRVoltage[MAXCHANNELNO];//临时用于处理ACR电压问题，若有其他衍生问题请删除
    static unsigned char DAC_Flag;
    static UWord32 WaitTime;
    static unsigned char WaitRecord;

    static unsigned long ACR_IHighDacCode;
    static unsigned long ACR_ILowDacCode;

    static UWord32 ACR_T1[MAXCHANNELNO];

    static unsigned char ACRTimerStatus[MAXCHANNELNO];
    static unsigned char ACRChannelNo;
    static unsigned short ACRChannelFlg;
    static unsigned char ACR_TimerEn;
    // 	static unsigned char CheckACRChannel;
    static unsigned char ACR_Calibrate[MAXCHANNELNO];

private:
    static char (* pACR_ADC)(char uc_ChannelNo);    //ACR中多板支持函数调用
    static char (*pRenew_ACR_ADC)(void);
    static void (*pACR_Read_ADC)(char uc_ChannelNo,unsigned long *SampleV,unsigned long *SampleI);
    //static void ACR_Calculate(unsigned char uc_ChannelNo);
    static signed int  TimerChannel_Check(unsigned char uc_CtrlType);
    static void Lock_QueueClear(unsigned char uc_ChannelNo);

public:
    static void Init(void);
    static void InitACR(unsigned char uc_ChannelNo);  //ACR初始化
    static void Calibrate_InitACR(unsigned char uc_ChannelNo);
    static void ACR_Calculate(unsigned char uc_ChannelNo);
    static void ProcessACR(unsigned char uc_ChannelNo);  //ACR处理
    static void ProcessACRTimer1(void); //ACR定时器中断处理
    static signed int  TimerChannel_Out(unsigned char uc_CtrlType);
    static unsigned char TimerChannel_Put(unsigned char uc_ChannelNo,unsigned char uc_CtrlType);
    static signed int Lock_Clear(unsigned char uc_ChannelNo,unsigned char uc_CtrlType);
    static void WriteChannel(unsigned char ChannelIdx,unsigned char SetFlag);
    static void Write_V(unsigned char ChannelIdx,unsigned char SetFlag);
};
#endif
