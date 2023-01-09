

#pragma once


#define BREAKNET_NUMBER  (10)  // SCH 状态定阅报告 断网计算器次数。

class ScheduleStatusReport
{
public:
    // 		static unsigned short               Priority;
    // 	    static unsigned short               TaskID;
    static unsigned char                uc_All_ReportFlag;       //  全部通道状态记录开关
    static unsigned char                uc_uc_BreakNet_Counter;               //  断网计算器
    static float                        m_StepCtrlValue[MAXCHANNELNO];        //  当前步计算的控制值

private:
    static unsigned char                uc_Ch_ReportFlag[MAXCHANNELNO];       //  通道状态记录开关
    static UWord32                      uw32_ChInterval[MAXCHANNELNO];        //  设定通道状态记录间隔时间
    static MsTime                       m_ChInterval_Report[MAXCHANNELNO];    //  通道状态记录间隔时间

public:
    static void Init(void);
    //inline static void DoWork(void);
    //inline static void SCHStatusReportDoWork(void);
    static void SCHStatusReport_DoWork(void);
    static void Set_ChIntervalTime(unsigned short ChID ,unsigned short Set_Time);

private:

};

