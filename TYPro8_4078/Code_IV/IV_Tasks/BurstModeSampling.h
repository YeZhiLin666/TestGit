

/*
任务一：采样任务

adc SampleProcess_period 进行周期采样，参考.net的原先设计：比如1ms 1次采样，每次采样1个adc通道

总体完成80%
*/

//BurstModeSampling
#ifndef _BURSTMODESAMPLING_H_
#define _BURSTMODESAMPLING_H_

#include "../../COMMON_OS/Protocols/SF_Hwf.h"


#define BURSTMODESAMPLING_MAX_CHANNEL   1       // 最大X个通道      // MAXCHANNELNO
#define BURSTMODESAMPLING_MAX_BUFFER    1000    // 最大X个缓冲区
#define BURSTMODESAMPLING_MAX_DELAY     1000    // 最大延时1s
#define BURSTMODESAMPLING_MIN_SENDDATA  54 //10      // 最小发送数据量


typedef struct
{
    unsigned long uw32_Voltage;
    unsigned long uw32_Current;

} st_SimpleData;

typedef CIRCULAR_QUEUE(st_SimpleData,BURSTMODESAMPLING_MAX_BUFFER)   St_SIMPLELIST_QUEUE;

class BurstModeSampling
{
public:
    //static UInt16 Priority;
    //static UInt16 TaskID;			//  should be a definition which positioning this task in OS_Task[]

    static bool  uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];    //  进入BurstMode模式true
    static bool  bl_NeedRunning[BURSTMODESAMPLING_MAX_CHANNEL];    // 条件合适需要运行
    static bool  bl_StepResume[BURSTMODESAMPLING_MAX_CHANNEL];     // 步恢复运行
    static bool  b_Logging[BURSTMODESAMPLING_MAX_CHANNEL];
    static bool  b_StepTimeReset[BURSTMODESAMPLING_MAX_CHANNEL];
    static unsigned short int uw16_BurstModeDelayTime[BURSTMODESAMPLING_MAX_CHANNEL];      //  PC下载BurstMode延迟时间 1:100us 单位：100us [允许输入0-10000之间]
    static unsigned long int  uw32_BurstModePointNum[BURSTMODESAMPLING_MAX_CHANNEL];       //  PC下载BurstMode打点总数  [允许输入100-1000之间]
    static unsigned long int  uw32_BurstModeSampleSpeed[BURSTMODESAMPLING_MAX_CHANNEL];    //  PC下载BurstMode打点速度 1:100us记录一点 n:n*100us记录一点 [允许输入1-10之间]
    static bool  bl_ChangedTime[BURSTMODESAMPLING_MAX_CHANNEL];    // 强变换时间标志
    static bool  bl_Time;           // 时间备份标志
    static MsTime OSTime_back;      // 系统时间备份标志
    static MsTime StartTime[BURSTMODESAMPLING_MAX_CHANNEL];                    //   BurstMode开始时间
//     static float fStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
//     static float fStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
    static double dStartTestTime[BURSTMODESAMPLING_MAX_CHANNEL];
    static double dStartStepTime[BURSTMODESAMPLING_MAX_CHANNEL];
// 	static unsigned int u32_burst_tick;
// 	static bool b_burst_start;
// 	static bool init_Start_Time;

    static void Init(void);
// 	static void DoWork(void);
// 	static void EndTask(void);

    static void ReportSimpleData(void);
    static unsigned char BurstReport_LogSDL(unsigned short ChannelIdx, MP_REPORT_SDL_4BYTESCYCLEIdx_PACK *pReport); //mits8
    static void BurstSavePoint(unsigned short ChannelIdx);
    static void OpenBurstModeSampling(unsigned short ChannelIdx);
    static void CloseBurstModeSampling(unsigned short ChannelIdx);
    static void BurstModeSamplingCheck(unsigned short ChannelIdx,unsigned short u16_StepID);
//     static void BurstModeSamplingResume(unsigned short ChannelIdx, double dSetValue);

//protected:

private:

    //static SimpleData  st_BurstSampleList[BURSTMODESAMPLING_MAX_BUFFER];
    static St_SIMPLELIST_QUEUE  st_BurstSampleList[BURSTMODESAMPLING_MAX_CHANNEL];    // MAXCHANNELNO
    static unsigned long int uw32_P_A_Log[BURSTMODESAMPLING_MAX_CHANNEL];      //  记录下一个数据位置
    static unsigned long int uw32_P_B_Send[BURSTMODESAMPLING_MAX_CHANNEL];     //  上报下一个数据位置
    //static bool uc_A_BurstMode[BURSTMODESAMPLING_MAX_CHANNEL];                 //  进入BurstMode模式true
    static bool uc_B_DataFinish[BURSTMODESAMPLING_MAX_CHANNEL];                //  上报数据完毕true
    //static MsTime StartTime[BURSTMODESAMPLING_MAX_CHANNEL];                    //   BurstMode开始时间

    //static unsigned long int  uw32_SendTotal[BURSTMODESAMPLING_MAX_CHANNEL];   //  已上报数据总量
    static unsigned char uc_Voltage_range[BURSTMODESAMPLING_MAX_CHANNEL];   // 记录数据的电压量程
    static unsigned char uc_Current_range[BURSTMODESAMPLING_MAX_CHANNEL];   // 记录数据的电流量程
    static unsigned short int  uw16_IntervalCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // 计算间隔时间
    static unsigned short int  uw16_SendTimeCounter[BURSTMODESAMPLING_MAX_CHANNEL];    // 发送间隔时间

    static void BurstPointChange(unsigned short ChannelIdx,st_SimpleData * S, St_Data_Pairs_4BytesCycleIdx * D); //mits8
    static void BurstParameterCheck(unsigned short ChannelIdx);

};

#endif
