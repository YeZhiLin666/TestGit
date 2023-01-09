///////////////////////////////////////////////////////////////////////////////////
//File Name: Step.h
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						Step管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../IV_Tasks/OS_LogicProcess.h"

class Check_Limit8_task
{
public:
    static UInt16 TaskID;                                                                    //任务ID
    static UInt16 Priority;                                                                  //优先级

    static unsigned char m_CheckedChannelCount_SL;
    static unsigned char m_CheckedChannelCount_LL;

public:
    static void        DoWork(void);                                                                 //任务执行函数
    static void        MyInit(void);                                                                 //任务初始化
// 	static void        EndTask(void);                                                                //任务结束
};


