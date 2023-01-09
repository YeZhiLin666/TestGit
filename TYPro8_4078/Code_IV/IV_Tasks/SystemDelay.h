///////////////////////////////////////////////////////////////////////////////////
//File Name: SystemDelay.h
//Version:		V1.0
//Date:				2015/9/8
//Description:
//						Step管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once

enum
{
    SYSTEMDELAY_IDLE=0,
    SYSTEMDELAY_WAIT,
    SYSTEMDELAY_FINISH,
    SYSTEMDELAY_REST
};
class SystemDelayTask
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级
    static unsigned char beSystemDelayFlg;
    static unsigned long m_DelayTime;                                                       //delay time
    static unsigned char SystemDelayStatus;
    static unsigned char TriggerDelay;
    static MsTime   m_RecordDelayTime;
    static unsigned char RestDelay;
public:
    static void        DoWork(void);                                                                 //任务执行函数
    static void        MyInit(void);                                                                 //任务初始化
    static void        EndTask(void);                                                                //任务结束
private:
    static void			GotoDelayWork(void);

};
